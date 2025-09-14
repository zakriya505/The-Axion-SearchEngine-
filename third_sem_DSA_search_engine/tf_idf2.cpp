#include <iostream>
#include <fstream>
#include <string>
#include <unordered_set>
#include <nlohmann/json.hpp>
using namespace std;

using json = nlohmann::json;

// Node Structure
struct Node
{
    string data;   // Store the string
    double tf_idf; // Store the TF-IDF value
    int repetition_count;
    Node *next; // Pointer to the next node

    Node(string data, double tf_idf)
        : data(data), tf_idf(tf_idf), repetition_count(0), next(nullptr) {}

    Node(string data, double tf_idf, int repetition_count)
        : data(data), tf_idf(tf_idf), repetition_count(repetition_count), next(nullptr) {}
};

// LinkedListOfIds Class
class LinkedListOfIds
{
public:
    Node *head; // Pointer to the first node in the linked list

    LinkedListOfIds() : head(nullptr) {}


    void insert_in_descending_order(string& data, double tf_idf, int repetetion_count)
    {
        Node *newNode = new Node(data, tf_idf, repetetion_count);

        // Case 1: Insert the new node at the beginning if the list is empty
        if (!head)
        {
            head = newNode;
            return;
        }

        // Case 2: Traverse the list to find the right place to insert
        Node *current = head;

        // First, we will compare based on repetition_count (descending order)
        // If repetition_count is the same, we will then compare based on tf_idf (descending order)
        if (newNode->repetition_count > current->repetition_count ||
            (newNode->repetition_count == current->repetition_count && newNode->tf_idf > current->tf_idf))
        {
            // Insert at the beginning if the new node has higher repetition_count or same repetition_count but higher tf_idf
            newNode->next = head;
            head = newNode;
        }
        else
        {
            // Traverse the list and find the right place to insert
            while (current->next &&
                   (current->next->repetition_count > newNode->repetition_count ||
                    (current->next->repetition_count == newNode->repetition_count && current->next->tf_idf > newNode->tf_idf)))
            {
                current = current->next;
            }
            newNode->next = current->next;
            current->next = newNode;
        }
    }

    void insert(string data, double tf_idf)
    {
        Node *newNode = new Node(data, tf_idf);

        // Case 1: If the list is empty, insert the new node as the head
        if (!head)
        {
            head = newNode;
            return;
        }

        // Case 2: Traverse the list to find if the node with the same ID exists
        Node *current = head;
        while (current)
        {
            if (current->data == data)
            {                              // If the ID exists
                current->tf_idf += tf_idf; // Add the new tf_idf to the existing node
                current->repetition_count++;
                delete newNode; // No need to keep the new node
                return;
            }
            current = current->next;
        }

        // Case 3: Insert the new node at the end if the ID doesn't exist
        current = head;
        while (current->next)
        { // Traverse until the last node
            current = current->next;
        }
        current->next = newNode; // Insert at the end
    }

    bool ISEMPTY()
    {
        if (head == nullptr)
        {
            return true;
        }
        return false;
    }

    // Function to pop the node with the highest tf_idf value (returns and deletes it)
    // Node *pop()
    // {
    //     if (!head)
    //     {
    //         cout << "List is empty, nothing to pop." << endl;
    //         return nullptr;
    //     }

    //     // Save the node to return
    //     Node *temp = head;

    //     // Move the head to the next node (remove the head)
    //     head = head->next;

    //     // Delete the popped node to free memory
    //     delete temp; // Deleting the node to free memory

    //     // Return the popped node (no need to set temp->next to nullptr anymore)
    //     return temp;
    // }
    // Function to pop the node with the highest tf_idf value (returns and deletes it)
    Node* pop()
    {
        if (!head)
        {
            cout << "List is empty, nothing to pop." << endl;
            return nullptr;
        }

        // Save the current head node to return it later
        Node* temp = head;

        // Move the head to the next node
        head = head->next;

        // Disconnect the old head node from the list
        temp->next = nullptr;

        // Return the old head node (caller must handle its deletion)
        return temp;

    }

    // Function to print the list (for testing purposes)
    void printList() const
    {
        Node *current = head;
        while (current)
        {
            // cout << "Data: " << current->data << ", TF-IDF: " << current->tf_idf << ", count: " << current->repetition_count << endl;
            current = current->next;
        }
    }
};

// Function to load stop words from the stopword.json file and save them into a new JSON file (hhh.json)
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

    // Debugging: Print the loaded JSON object
    // cout << "Loaded JSON: " << j.dump(4) << endl;

    // Assuming stopwords are in a JSON array
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

    json output_json;
    output_json["stopwords"] = json::array();

    for (const string &stop_word : stop_words)
    {
        output_json["stopwords"].push_back(stop_word);
    }

    return stop_words;
}

// Function to load strings from a JSON array
unordered_set<string> load_lexicon(const string &json_file)
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
    catch (const json::parse_error &e)
    {
        cerr << "Error: Failed to parse JSON file. " << e.what() << endl;
        return result;
    }

    // Ensure it's an array and extract the strings
    if (j.is_array())
    {
        for (const auto &item : j)
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

// Helper function to split a string into words (for abstract processing)
vector<string> split_into_words(const string &str)
{
    vector<string> words;
    stringstream ss(str);
    string word;

    // Split string into words by spaces
    while (ss >> word)
    {
        // Remove any leading/trailing spaces and make it lowercase for consistent comparison
        transform(word.begin(), word.end(), word.begin(), ::tolower); // Convert to lowercase
        words.push_back(word);
    }

    return words;
}

// Function to read JSON file into an unordered_map
// Function to load the JSON data into an unordered_map
unordered_map<string, vector<string>> load_inverted_index(const string &json_file)
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
    catch (const json::parse_error &e)
    {
        cerr << "Error: Failed to parse JSON file. " << e.what() << endl;
        return result;
    }

    // Ensure it's an object and process the data
    if (j.is_object())
    {
        for (auto &[key, value] : j.items())
        {
            if (value.is_array())
            {
                vector<string> identifiers;
                for (const auto &item : value)
                {
                    if (item.is_string())
                    {
                        identifiers.push_back(item.get<string>());
                    }
                }
                result[key] = identifiers; // Store the key-value pair in the map
            }
        }
    }
    else
    {
        cerr << "Error: The JSON file is not in the expected object format." << endl;
    }

    return result;
}

// Function to load the forward index from a JSON file
unordered_map<string, unordered_map<string, int>> load_forward_index(const string &forward_index_json_file)
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
    catch (const json::parse_error &e)
    {
        cerr << "Error parsing JSON: " << e.what() << endl;
        return forward_index;
    }

    // Convert JSON to unordered_map
    try
    {
        for (auto &[key, value] : j.items())
        {
            unordered_map<string, int> inner_map;
            for (auto &[inner_key, freq] : value.items())
            {
                inner_map[inner_key] = freq.get<int>();
            }
            forward_index[key] = inner_map;
        }
    }
    catch (const exception &e)
    {
        cerr << "Error converting JSON to unordered_map: " << e.what() << endl;
    }

    return forward_index;
}

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
    DataEntry(const string &t, const string &a, const vector<string> &u)
        : title(t), abstract(a), urls(u) {}

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
        for (const auto &url : urls)
        {
            cout << url << " ";
        }
        cout << endl;
    }
};

// Function to load data from the JSON file and store it in a map
void load_document_data(const string &filename, unordered_map<string, DataEntry> &documents)
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
    for (auto &entry : jsonData.items())
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

int hashfunction_for_doc(string key)
{
    std::hash<std::string> hash; // Create a hash function object for string
    return hash(key) % 50;
}

int main()
{
    // string inverted_index_json_file = "inverted_index.json";
    string inverted_index_json_file;
    string lexicon = "__lexicon50out.json";
    // string lexicon = "lexicon.json";
    string forward_index_json_file = "py_FIVX.json";// forward index is here
    // string forward_index_json_file = "forward_index256.json";

    unordered_set<string> stop_words = load_stop_words("combined_stopwords.json");

    // unordered_map<string, DataEntry> documents; // Map to store documents by their ID
    // Loop to load data into each unordered_map
    unordered_map<string, DataEntry> documents[50];


    for (int i = 0; i < 50; ++i)
    {
        string filename = "./document_barels/output_barrel_" + to_string(i) + ".json"; // Generate file name dynamically
        load_document_data(filename, documents[i]);                                    // Load data into the corresponding unordered_map
        cout << "the document barel" << i << "loaded\n";
    }

    unordered_map<string, vector<string>> inverted_index[50];

    for (int i = 0; i < 50; i++)
    {
        string inverted_index_json_file = "./maker/inverted_index_bucket_" + to_string(i) + ".json";
        // string inverted_index_json_file = "./inverted_index_barels/inverted_index_bucket_" + to_string(i) + ".json";
        cout << "the inverted index barel" << i << "loaded\n";
        inverted_index[i] = load_inverted_index(inverted_index_json_file);
    }
    // ifstream inverted_index_file(inverted_index_json_file);
    // if (!inverted_index_file.is_open())
    // {
    //     cerr << "Could not open the data file!" << endl;
    //     return 1;
    // }

    ifstream lexicon_file(lexicon);
    if (!lexicon_file.is_open())
    {
        cerr << "Could not open the data file!" << endl;
        return 1;
    }

    unordered_set<string> lexicon_set = load_lexicon(lexicon);
    unordered_map<string, unordered_map<string, int>> forward_indxex = load_forward_index(forward_index_json_file);

    cout <<" all the docuemnt loaded ,ok ;;\n" ;

    vector<string> result_of_search;

    double word_freq_in_fwd_index_id;
    double total_freq_in_fwd_index_id;
    double tf;
    double idf;
    double tf_idf;
    double number_of_documents_in_db = 50000; // please give the totla number of documents in the db
    double size_of_ids_in_word;

    LinkedListOfIds list_of_ids1;
    LinkedListOfIds list_of_ids2;

    cout <<" all the docuemnt loaded ,and objects mae \n" ;

    while (true)
    {

        string input_by_user;
        cout << "Enter what you want to search : ";
        getline(cin, input_by_user); // Reads until a newline character

        // now we need to tokenize it and then rmove the stop words, adn then ,
        vector<string> words = split_into_words(input_by_user);
        vector<string> our_query;

        for (string &word : words)
        {
            if (stop_words.find(word) == stop_words.end()) // that mean the word is not present it the stop word file so we can go furthur and use it
            {
                if (lexicon_set.find(word) == lexicon_set.end()) // same here but reverse , we wana use words present in the lexicon , so if the word not present we continue to next word
                {
                    continue;
                }
                our_query.push_back(word);
            }
        }

        for (string &word : our_query)
        {
            int inverted_index_bucket = hashfunction_for_doc(word);
            cout<<"the word is : "<<word << " and the inverted index bucket is : " << inverted_index_bucket << endl;
            auto ids = inverted_index[inverted_index_bucket].find(word);

            size_of_ids_in_word = ids->second.size();
            // cout << "Number of IDs for............ '" << word << "' is: " << size_of_ids_in_word << endl;

            if (ids != inverted_index[inverted_index_bucket].end())
            {
                // If key is found, print its values

                for (const string &value : ids->second)
                {

                    // cout << value << " ";
                    // now for each value calculte the , tf - idf , for the tf we need to access the forward index ;
                    // Check if the key exists
                    if (forward_indxex.find(value) != forward_indxex.end())
                    {
                        const auto &inner_map = forward_indxex[value];

                        // Check if the word exists under the key
                        if (inner_map.find(word) != inner_map.end())
                        {
                            // cout << "The word '" << word << "' is present with frequency: "
                            //      << inner_map.at(word) << endl;
                            word_freq_in_fwd_index_id = inner_map.at(word);
                        }
                        else
                        {
                            cout << "The word '" << word << "' is not present in the key '"
                                 << value << "'." << endl;
                        }

                        // Calculate the total frequency for the key
                        int total_frequency = 0;
                        for (const auto &[inner_word, freq] : inner_map)
                        {
                            total_frequency += freq;
                        }

                        // cout << "The total frequency for the key '" << value << "' is: "
                        //      << total_frequency << endl;
                        total_freq_in_fwd_index_id = total_frequency;
                    }
                    else
                    {
                        cout << "The key '" << value << "' is not present in the data." << endl;
                    }

                    //  now as te value is found at its , tf fid out ;
                    tf = (word_freq_in_fwd_index_id / total_freq_in_fwd_index_id); // as the tf is presen know fid the idf
                    idf = (number_of_documents_in_db / (1 + size_of_ids_in_word));

                    tf_idf = tf * idf;

                    // now create nodes based on the id and the tf_idf
                    // cout << " the tf_idf fo value : " << value << " is the : " << tf_idf << endl;

                    list_of_ids1.insert(value, tf_idf);
                }
                cout << endl;
            }
        }

        // now we have words that we wana search :

        // cout << "  now we have the words , but we still need to do the , lamitizatoin , but what even ,\n know go to the inverted index \n \n";
        // cout << "  simple insertiong work \n \n";
        // list_of_ids1.printList();

        while (!list_of_ids1.ISEMPTY())
        {
            Node *data_node = list_of_ids1.pop();
            //    cout<< "value of : " << data_node->data << " is : " << data_node->tf_idf << ".......////////............" << endl ;
            list_of_ids2.insert_in_descending_order(data_node->data, data_node->tf_idf, data_node->repetition_count);
        }

        // cout << "/////////////////////////////// now we are printing list on the insert in descending order  ////////////////////////////////: \n";
        // list_of_ids2.printList();

        // now calculate the hash of ids

        json output_json;

        while (!list_of_ids2.ISEMPTY())
        {
            Node *data_node = list_of_ids2.pop();

            int barel = hashfunction_for_doc(data_node->data);

            cout<<"the barel for the document is : " << barel << endl;

            // Check if the document exists at the specified index
            if (documents[barel].find(data_node->data) != documents[barel].end())
            {
                DataEntry doc = documents[barel][data_node->data];

                // Add document details to JSON
                json doc_json = {
                    {"Title", doc.getTitle()},
                    {"Abstract", doc.getAbstract()},
                    {"URLs", doc.getUrls()}};
                output_json.push_back(doc_json);
            }
            else
            {
                // Add error message for not found document
                output_json.push_back({{"Error", "Document not found"},
                                       {"ID", data_node->data}});
            }
        }

        // Save JSON to a file
        ofstream output_file("final_output.json");
        if (output_file.is_open())
        {
            output_file << output_json.dump(4); // Pretty-print with 4-space indentation
            output_file.close();
        }
        else
        {
            cout << "Error opening file to save JSON." << endl;
        }
    }

    // now we need to calcultae the hash of the string , and go to the document and search it

    // thier is some issue here , liek , in the ranking the consider i have searched the word project of algorithm , their is a case in which , consider  even if a document like having a
    // word of project nad algo , it is having less conbined tf_idf , as compare to the word project , or agorithm ,   this is jsut a hunch may be but
    // it can be possible like hier is no documnet , having the both words combine so check it
    // we can do like we put the ids that are repeating  , mean they have both of the words we are entering , they get the priority
    // consider we enter  4 to 5 words  , and one document has 3 words present in it we entered , adn one has all the main words we entered , so we give priority to the document hasivn all the 4 words , in it
    // well this is also a guess we have to hit and trial for it

    // one approach left is , use the tf_idf and , find the repetitoin nad tf_idf , and multiply them , liek   tf_idf * (1+log(repetetion))
}

