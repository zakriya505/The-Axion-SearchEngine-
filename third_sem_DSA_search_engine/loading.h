#pragma once

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <nlohmann/json.hpp>
#include <fstream>


using namespace std;
using json = nlohmann::json;


// Class to hold the information for each entry
class DataEntry
{
private:
    string title;
    string abstract;
    vector<string> urls;

public:
    // Default constructor
    DataEntry() = default;


    // Parameterized constructor
    DataEntry(const string& t, const string& a, const vector<string>& u)
        : title(t), abstract(a), urls(u) {}

    // Copy constructor (if required for mapping or custom objects)
    DataEntry(const DataEntry& other)
        : title(other.title), abstract(other.abstract), urls(other.urls) {}

    // Getter for title
    string getTitle() const
    {
        return title;
    }

    // Getter for abstract
    string getAbstract() const
    {
        return abstract;
    }

    // Getter for URLs
    vector<string> getUrls() const
    {
        return urls;
    }

    // Method to display the document details
    void display() const
    {
        cout << "Title: " << title << endl;
        cout << "Abstract: " << abstract << endl;
        cout << "URLs: ";
        for (const auto& url : urls)
        {
            cout << url << " ";
        }
        cout << endl;
    }
};


// Declare global variables (objects and arrays)
std::unordered_map<std::string, DataEntry> documents[50];  // Array of unordered_map for documents
std::unordered_map<std::string, std::vector<std::string>> inverted_index[50];  // Array of unordered_map for inverted index
std::unordered_set<std::string> stop_words;  // Set of stop words
std::unordered_set<std::string> lexicon_set;  // Set of lexicon words
std::unordered_map<std::string, std::unordered_map<std::string, int>> forward_index;  // Forward index map


// Function to load stop words from the stopword.json file and save them into a new JSON file (hhh.json)
unordered_set<string> load_stop_words(const string& stopword_file)
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

    // Debugging: Print the loaded JSON object
    // cout << "Loaded JSON: " << j.dump(4) << endl;

    // Assuming stopwords are in a JSON array
    if (j.is_object() && j.contains("stopwords") && j["stopwords"].is_array())
    {
        for (const auto& word : j["stopwords"])
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

    json output_json;
    output_json["stopwords"] = json::array();

    for (const string& stop_word : stop_words)
    {
        output_json["stopwords"].push_back(stop_word);
    }

    return stop_words;
}

// Function to load strings from a JSON array
unordered_set<string> load_lexicon(const string& json_file)
{
    unordered_set<string> result;

    // Open the JSON file
    ifstream file(json_file);
    if (!file.is_open())
    {
        cerr << "Error: Could not open the JSON file: " << json_file << endl;
        return result;
    }

    // Parse the JSON
    json j;
    try
    {
        file >> j;
    }
    catch (const json::parse_error& e)
    {
        cerr << "Error: Failed to parse JSON file. " << e.what() << endl;
        return result;
    }

    // Ensure it's an array and extract the strings
    if (j.is_array())
    {
        for (const auto& item : j)
        {
            if (item.is_string())
            {
                string word = item.get<string>();
                transform(word.begin(), word.end(), word.begin(), ::tolower); // Convert to lowercase
                if (!word.empty())
                { // Skip empty strings
                    result.insert(word);
                }
            }
        }
    }
    else
    {
        cerr << "Error: The JSON does not contain an array structure." << endl;
    }

    return result;
}

// Function to load data from the JSON file and store it in a map
void load_document_data(const string& filename, unordered_map<string, DataEntry>& documents)
{
    ifstream file(filename); // Open the JSON file
    if (!file.is_open())
    {
        cerr << "Could not open the file: " << filename << endl;
        return;
    }

    json jsonData;
    file >> jsonData; // Parse the JSON data into a json object

    // Iterate over each entry in the JSON file
    for (auto& entry : jsonData.items())
    {
        string key = entry.key(); // Document ID
        string title = entry.value()["title"];
        string abstract = entry.value()["abstract"];
        vector<string> urls = entry.value()["urls"].get<vector<string>>(); // Extract URLs

        DataEntry dataEntry(title, abstract, urls); // Create DataEntry object
        documents[key] = dataEntry;                 // Store in the unordered_map (key = document ID)
    }

    file.close(); // Close the file
}


// Function to load the forward index from a JSON file
unordered_map<string, unordered_map<string, int>> load_forward_index(const string& forward_index_json_file)
{
    unordered_map<string, unordered_map<string, int>> forward_index;

    // Open the file
    ifstream file(forward_index_json_file);
    if (!file.is_open())
    {
        cerr << "Could not open the forward index file: " << forward_index_json_file << endl;
        return forward_index;
    }

    // Parse JSON
    json j;
    try
    {
        file >> j;
    }
    catch (const json::parse_error& e)
    {
        cerr << "Error parsing JSON: " << e.what() << endl;
        return forward_index;
    }

    // Convert JSON to unordered_map
    try
    {
        for (auto& [key, value] : j.items())
        {
            unordered_map<string, int> inner_map;
            for (auto& [inner_key, freq] : value.items())
            {
                inner_map[inner_key] = freq.get<int>();
            }
            forward_index[key] = inner_map;
        }
    }
    catch (const exception& e)
    {
        cerr << "Error converting JSON to unordered_map: " << e.what() << endl;
    }

    return forward_index;
}

// Function to load the JSON data into an unordered_map
unordered_map<string, vector<string>> load_inverted_index(const string& json_file)
{
    unordered_map<string, vector<string>> result;

    // Open the JSON file
    ifstream file(json_file);
    if (!file.is_open())
    {
        cerr << "Error: Could not open the JSON file: " << json_file << endl;
        return result;
    }

    // Parse the JSON file
    json j;
    try
    {
        file >> j;
    }
    catch (const json::parse_error& e)
    {
        cerr << "Error: Failed to parse JSON file. " << e.what() << endl;
        return result;
    }

    // Ensure it's an object and process the data
    if (j.is_object())
    {
        int c = 0;
        for (auto& [key, value] : j.items())
        {
            if (value.is_array())
            {
                vector<string> identifiers;
                for (const auto& item : value)
                {
                    if (item.is_string())
                    {
                        identifiers.push_back(item.get<string>());
                        
                    }
                }
                result[key] = identifiers; // Store the key-value pair in the map
            }
            c++;


            //cout << "Loadeddddddddddd" + c  << endl; 
        }
    }
    else
    {
        cerr << "Error: The JSON file is not in the expected object format." << endl;
    }

    return result;
}


void load_all_json_datas()
{
    //first load stop words
    // lexicon
    // document data
    // forward index 
    // inverted index 

    stop_words = load_stop_words("D:/3rd Sem/DSA/Project/to_send/combined_stopwords.json");
    // string lexicon = "D:/3rd Sem/DSA/Project/to_send/lexicon.json";
    string lexicon = "D:\\3rd Sem\\DSA\\Project\\to_send\\__lexicon3__out.json";

     lexicon_set = load_lexicon(lexicon);

     for (int i = 0; i < 50; ++i)
     {
         string filename = "D:/3rd Sem/DSA/Project/to_send/document_barels/output_barrel_" + to_string(i) + ".json"; // Generate file name dynamically
         load_document_data(filename, documents[i]);                                    // Load data into the corresponding unordered_map
         cout << "the barel" << i << "loaded\n";
     }

     /*
     // Load documents into an array of unordered_maps
     for (int i = 0; i < 50; ++i)
     {
         string filename = "./document_barels/output_barrel_" + to_string(i) + ".json";
         documents[i] = load_document_data(filename);
         cout << "The barrel " << i << " loaded\n";
     }*/

     //string forward_index_json_file = "D:/3rd Sem/DSA/Project/to_send/forward_index256.json";
     string forward_index_json_file = "D:\\3rd Sem\\DSA\\Project\\to_send\\py_FIVX__.json";

     forward_index = load_forward_index(forward_index_json_file);

     for (int i = 0; i < 50; i++)
     {
         string inverted_index_json_file = "D:/3rd Sem/DSA/Project/to_send/inverted_index_barels/inverted_index_bucket_" + to_string(i) + ".json";
         inverted_index[i] = load_inverted_index(inverted_index_json_file);
         cout << "inv indx " << i << " loaded\n";
     }


}