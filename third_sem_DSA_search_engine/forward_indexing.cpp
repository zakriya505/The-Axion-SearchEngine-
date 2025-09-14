#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <regex>

using json = nlohmann::json;
using namespace std;

// Helper function to split a string into words (for abstract processing)
vector<string> split_into_words(const string &str)
{
    vector<string> words;
    stringstream ss(str);
    string word;

    std::regex wordRegex(R"([a-zA-Z0-9]+)");

    // Use regex_iterator to extract all individual words
    std::sregex_iterator wordsBegin(str.begin(), str.end(), wordRegex);
    std::sregex_iterator wordsEnd;

    for (std::sregex_iterator it = wordsBegin; it != wordsEnd; ++it)
    {
        std::string word = it->str();

        // Convert the word to lowercase for consistent comparison
        std::transform(word.begin(), word.end(), word.begin(), ::tolower);

        // Add the word to the token list
        words.push_back(word);
    }

    return words;
}

// Function to clean a word by removing non-alphabetical characters (using regex)
string clean_word(const string &word)
{
    string cleaned_word = word;
    // cleaned_word = regex_replace(cleaned_word, regex("[^a-zA-Z]"), "");
    return cleaned_word;
}

// A function to lemmatize a word by removing common verb and plural noun suffixes
string lemmatize_word(const string &word)
{
    string w = word;
    transform(w.begin(), w.end(), w.begin(), ::tolower);

    static unordered_map<string, string> irregular_verbs = {
        {"ate", "eat"}, {"went", "go"}, {"had", "have"}, {"did", "do"}, {"was", "be"}, {"were", "be"}};

    if (irregular_verbs.find(w) != irregular_verbs.end())
    {
        return irregular_verbs[w];
    }

    if (w.length() > 3)
    {
        if (w.substr(w.length() - 3) == "ing")
        {
            string root = w.substr(0, w.length() - 3);
            if (root.length() > 2)
            {
                return root;
            }
        }
        if (w.substr(w.length() - 2) == "ed")
        {
            return w.substr(0, w.length() - 2);
        }
        if (w.substr(w.length() - 2) == "es")
        {
            return w.substr(0, w.length() - 2);
        }
    }

    if (w.length() > 2)
    {
        if (w.substr(w.length() - 1) == "s")
        {
            if (w.length() > 3 && w.substr(w.length() - 2) == "es")
            {
                return w.substr(0, w.length() - 2);
            }
            return w.substr(0, w.length() - 1);
        }
    }

    return w;
}

// Function to load stop words from the stopword.json file
unordered_set<string> load_stop_words(const string &stopword_file)
{
    unordered_set<string> stop_words;
    ifstream file(stopword_file);
    if (!file.is_open())
    {
        cerr << "Could not open the stopword file!" << endl;
        return stop_words;
    }

    json j;
    file >> j;

    if (j.is_object() && j.contains("stopwords") && j["stopwords"].is_array())
    {
        for (const auto &word : j["stopwords"])
        {
            if (word.is_string())
            {
                string stop_word = word.get<string>();
                transform(stop_word.begin(), stop_word.end(), stop_word.begin(), ::tolower);
                stop_words.insert(stop_word);
            }
        }
    }
    else
    {
        cerr << "Error: The JSON file does not contain the expected structure." << endl;
    }

    return stop_words;
}

int main()
{
    string stopword_file = "combined_stopwords.json";
    string data_file = "Tonelak_data.json";
    string output_file = "forward_index256.json";

    unordered_set<string> stop_words = load_stop_words(stopword_file);

    unordered_map<string, unordered_map<string, int>> forward_index;

    ifstream datafile(data_file);
    if (!datafile.is_open())
    {
        cerr << "Could not open the data file!" << endl;
        return 1;
    }

    json data_json;
    datafile >> data_json;

    for (size_t i = 0; i < data_json.size() && i < 1000; ++i)
    {
        unordered_map<string, int> term_frequency;

        if (data_json[i].contains("title"))
        {
            if (data_json[i]["title"].is_array())
            {

                for (const auto &title_part : data_json[i]["title"])
                {
                    if (title_part.is_string())
                    {
                        string title = title_part.get<string>();
                        vector<string> words = split_into_words(title);
                        for (const string &word : words)
                        {
                            string cleaned_word = clean_word(word);
                            if (!cleaned_word.empty() && stop_words.find(cleaned_word) == stop_words.end())
                            {
                                // string lemmatized_word = cleaned_word;
                                string lemmatized_word = lemmatize_word(cleaned_word);
                                term_frequency[lemmatized_word]++;
                            }
                        }
                    }
                }
            }
            else if (data_json[i]["title"].is_string())
            {
                string title = data_json[i]["title"].get<string>();
                vector<string> words = split_into_words(title);
                for (const string &word : words)
                {
                    string cleaned_word = clean_word(word);
                    if (!cleaned_word.empty() && stop_words.find(cleaned_word) == stop_words.end())
                    {
                        // string lemmatized_word = cleaned_word;
                        string lemmatized_word = lemmatize_word(cleaned_word);
                        term_frequency[lemmatized_word]++;
                    }
                }
            }
        }

        if (data_json[i].contains("abstract") && data_json[i]["abstract"].is_string())
        {
            string abstract_text = data_json[i]["abstract"].get<string>();
            vector<string> words = split_into_words(abstract_text);
            for (const string &word : words)
            {
                string cleaned_word = clean_word(word);
                if (!cleaned_word.empty() && stop_words.find(cleaned_word) == stop_words.end())
                {
                    // string lemmatized_word = cleaned_word;
                    string lemmatized_word = lemmatize_word(cleaned_word);
                    term_frequency[lemmatized_word]++;
                }
            }
        }

        if (data_json[i].contains("keywords"))
        {
            if (data_json[i]["keywords"].is_array())
            {
                for (const auto &keyword : data_json[i]["keywords"])
                {
                    if (keyword.is_string())
                    {
                        string word12 = keyword.get<string>();
                        vector<string> words = split_into_words(word12);
                        for (string word : words)
                        {
                            string cleaned_word = clean_word(word);
                            if (!cleaned_word.empty() && stop_words.find(cleaned_word) == stop_words.end())
                            {
                                // string lemmatized_word = cleaned_word;
                                string lemmatized_word = lemmatize_word(cleaned_word);
                                term_frequency[lemmatized_word]++;
                            }
                        }
                    }
                }
            }
            else if (data_json[i]["keywords"].is_string())
            {
                string word = data_json[i]["keywords"].get<string>();
                vector<string> words = split_into_words(word);
                for (string word : words)
                {
                    string cleaned_word = clean_word(word);
                    if (!cleaned_word.empty() && stop_words.find(cleaned_word) == stop_words.end())
                    {
                        // string lemmatized_word = cleaned_word;
                        string lemmatized_word = lemmatize_word(cleaned_word);
                        term_frequency[lemmatized_word]++;
                    }
                }
            }
        }

        if (data_json[i].contains("year"))
        {
            string year;
            if (data_json[i]["year"].is_number())
            {
                year = to_string(data_json[i]["year"].get<int>());
            }
            else if (data_json[i]["year"].is_string())
            {
                year = data_json[i]["year"].get<string>();
            }

            term_frequency[year]++;

        
        }

        if (data_json[i].contains("authors") && data_json[i]["authors"].is_array())
        {
            for (const auto &author : data_json[i]["authors"])
            {
                if (author.contains("name") && author["name"].is_string())
                {
                    string full_name = author["name"].get<string>();
                    vector<string> author_parts = split_into_words(full_name);

                    for (string part : author_parts)
                    {
                        string cleaned_part = clean_word(part);
                        if (!cleaned_part.empty() && stop_words.find(cleaned_part) == stop_words.end())
                        {
                            // string lemmatized_part = cleaned_part;
                            string lemmatized_part = lemmatize_word(cleaned_part);
                            term_frequency[lemmatized_part]++;
                        }
                    }
                }
            }
        }

        string id = data_json[i]["id"];
        forward_index[id] = term_frequency;
    }

    json output_json;
    for (const auto &[doc_id, terms] : forward_index)
    {
        json term_json;
        for (const auto &[term, frequency] : terms)
        {
            term_json[term] = frequency;
        }
        output_json[doc_id] = term_json;
    }

    ofstream output(output_file);
    if (!output.is_open())
    {
        cerr << "Could not open the output file!" << endl;
        return 1;
    }
    output << output_json.dump(4);

    cout << "Forward index saved to " << output_file << endl;

    return 0;
}
