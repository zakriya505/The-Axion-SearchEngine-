#include <iostream>
#include <fstream>
#include <string>
#include <unordered_set>
#include <nlohmann/json.hpp>
#include <sstream>
#include "C:\Users\Hanzla Kalim\source\repos\dsa\loading.h"
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
        cout << "we are in insert in desc\n";
        cout << "Data: " << data << ", TF-IDF: " << tf_idf << ", Repetition Count: " << repetetion_count << endl;
        Node *newNode = new Node(data, tf_idf, repetetion_count);

        cout << "we have made a new node  \n";
        // Case 1: Insert the new node at the beginning if the list is empty
        if (!head)
        {
            head = newNode;
            return;
        }
        cout << "we head returned \n";

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

 /*   // Function to pop the node with the highest tf_idf value (returns and deletes it)
    Node *pop()
    {
        if (!head)
        {
            cout << "List is empty, nothing to pop." << endl;
            return nullptr;
        }

        // Save the node to return
        Node* temp = head;

        // Move the head to the next node (remove the head)
        head = head->next;

        // Delete the popped node to free memory
        delete temp; // Deleting the node to free memory

        // Return the popped node (no need to set temp->next to nullptr anymore)
        return temp;

    }*/

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
            cout << "Data: " << current->data << ", TF-IDF: " << current->tf_idf << ", count: " << current->repetition_count << endl;
            current = current->next;
        }
    }
};


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
    cout << "I am here in slip function\n";

    return words;
}

/*
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

*/


// Class to hold the information for each entry
class DataEntry1
{
private:
    string title;
    string abstract;
    vector<string> urls;

public:
    // Default constructor
    DataEntry1() = default;

    // Parameterized constructor
    DataEntry1(const string &t, const string &a, const vector<string> &u)
        : title(t), abstract(a), urls(u) {}

    // Copy constructor (if required for mapping or custom objects)
    DataEntry1(const DataEntry1& other)
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
        for (const auto &url : urls)
        {
            cout << url << " ";
        }
        cout << endl;
    }
};



int hashfunction_for_doc(string key)
{
    hash<string> hash; // Create a hash function object for strings
    cout << "the key is :" << key << " and hash__ is : " << hash(key) << endl;
    return  hash(key) % 50;     // Compute the raw hash value

    //std::hash<std::string> hash;         // Create a hash function object for strings
    //size_t hashed_value = hash(key);     // Compute the raw hash value

    // Enhanced debug information
    //cout << "Raw hash value for key '" << key << "': " << hashed_value << endl;

    // Apply better mapping to buckets
    //int bucket_count = 50;               // Define the total number of buckets
    //int bucket = static_cast<int>(hashed_value % bucket_count);

    // Debug output
    //cout << "Mapped bucket for key '" << key << "': " << bucket << endl;

    //return bucket;
}



string lamitizedString(string input) {

    std::string input_text = input;

    std::string pythonFilePath = "D:/3rd Sem/DSA/Project/to_send/t2.py";

    // Create the command to run the Python script
    std::cout << "C++ File working\n";
    std::string command = "python \"" + pythonFilePath + "\" \"" + input_text + "\" > output.txt";

    // Run the Python script using system()
    std::cout << "Running command: " << command << std::endl;
    int ret = system(command.c_str());  // Execute the command

    // Check for errors in running the script
    //if (ret != 0) {
    //    std::cerr << "Error running Python script!" << std::endl;
    //    return 1;
    //}

    // Read the output from the file
    std::ifstream result_file("output.txt");
    //if (!result_file.is_open()) {
    //    std::cerr << "Error opening output file!" << std::endl;
    //    return 1;
    //}

    std::string result;
    std::string line;

    // Read the file line by line and store it in result
    while (getline(result_file, line)) {
        result += line + "\n";  // Add a newline to separate the lines
    }

    result_file.close();  // Close the file after reading

    // Print the lemmatized result
    std::cout << "Lemmatized Text in C++: " << result << std::endl;

    return result;

}

void TF_IDF(string input_by_user) // main // this one will take the string from user nad call a python code to lamitise it 
{
    vector<string> result_of_search;

    double word_freq_in_fwd_index_id;
    double total_freq_in_fwd_index_id;
    double tf;
    double idf;
    double tf_idf;
    double number_of_documents_in_db = 1000;
    double size_of_ids_in_word;
    
    
    LinkedListOfIds list_of_ids1;
    LinkedListOfIds list_of_ids2;

    
    
        
        input_by_user = lamitizedString(input_by_user);
        //string input_by_user;
        //cout << "Enter what you want to search : ";
        //getline(cin, input_by_user); // Reads until a newline character

        // now we need to tokenize it and then rmove the stop words, adn then ,
        cout << "I am here\n";
        vector<string> words = split_into_words(input_by_user);
        cout << "I am here after split function\n";
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

        cout << "I am here after for loop\n";

        for (string& word : our_query)
        {
            cout << "I am not going into the function\n";
            int inverted_index_bucket = hashfunction_for_doc(word);

            // Debug hash bucket and word
            cout << "Bucket for word '" << word << "': " << inverted_index_bucket << endl;
            cout << "Checking bucket contents before search...\n";

            // Print bucket contents
            for (const auto& pair : inverted_index[inverted_index_bucket]) {
                cout << "Word: " << pair.first << ", IDs count: " << pair.second.size() << endl;
            }

            cout << "Looking for the word in the bucket...\n";
            auto ids = inverted_index[inverted_index_bucket].find(word);

            if (ids != inverted_index[inverted_index_bucket].end()) {
                cout << "Found word: " << word << endl;
                cout << "Number of IDs for word '" << word << "': " << ids->second.size() << endl;

                size_of_ids_in_word = ids->second.size();
                cout << "Number of IDs for............ '" << word << "' is: " << size_of_ids_in_word << endl;

                if (ids != inverted_index[inverted_index_bucket].end())
                {
                    // If key is found, print its values

                    for (const string& value : ids->second)
                    {

                        cout << value << " ";
                        // now for each value calculte the , tf - idf , for the tf we need to access the forward index ;
                        // Check if the key exists
                        if (forward_index.find(value) != forward_index.end())
                        {
                            const auto& inner_map = forward_index[value];

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
                            for (const auto& [inner_word, freq] : inner_map)
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


                // now we have words that we wana search :

                cout << "  now we have the words , but we still need to do the , lamitizatoin , but what even ,\n know go to the inverted index \n \n";
                cout << "  simple insertiong work \n \n";
                list_of_ids1.printList();

                cout << "list of ids  1 is printed \n";

                while (!list_of_ids1.ISEMPTY())
                {
                    cout << "we are in the list of ids 1 foor loop \n";
                    Node* data_node = list_of_ids1.pop();
                    cout << "we poped the value \n";
                    //    cout<< "value of : " << data_node->data << " is : " << data_node->tf_idf << ".......////////............" << endl ;
                    list_of_ids2.insert_in_descending_order(data_node->data, data_node->tf_idf, data_node->repetition_count);
                }

                cout << "/////////////////////////////// now we are printing list on the insert in descending order  ////////////////////////////////: \n";
                list_of_ids2.printList();

                cout << "list of ids  2__ is printed \n";



                // now calculate the hash of ids

                json output_json;

                while (!list_of_ids2.ISEMPTY())
                {
                    Node* data_node = list_of_ids2.pop();

                    int barel = hashfunction_for_doc(data_node->data);

                    // Check if the document exists at the specified index
                    if (documents[barel].find(data_node->data) != documents[barel].end())
                    {
                        //DataEntry1 doc = documents[barel][data_node->data];
                        //const DataEntry1& doc = documents[barel].at(data_node->data);


                        // Add document details to JSON
                        json doc_json = {
                            {"Title",  documents[barel][data_node->data].getTitle()},
                            {"Abstract",  documents[barel][data_node->data].getAbstract()},
                            {"URLs",  documents[barel][data_node->data].getUrls()} };
                        output_json.push_back(doc_json);
                    }
                    else
                    {
                        // Add error message for not found document
                        output_json.push_back({ {"Error", "Document not found"},
                                               {"ID", data_node->data} });
                    }
                }

                // Save JSON to a file
                ofstream output_file("D:\\3rd Sem\\DSA\\Project\\to_send\\final_output.json");
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
        }
}