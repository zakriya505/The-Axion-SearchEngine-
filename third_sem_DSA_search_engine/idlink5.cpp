#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

// DataEntry Class: Holds the information for title, abstract, and URLs
class DataEntry
{
public:
    string title;
    string abstract;
    vector<string> urls;
    // vector<string> urls;

    DataEntry() = default;

    DataEntry(const string &t, const string &a, const vector<string> &u)
        : title(t), abstract(a), urls(u) {}

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

// Node Class: Represents a node in the linked list (used for collision handling)
class Node
{
public:
    string key;      // Key for the HashMap (id)
    DataEntry value; // Associated value (DataEntry object)
    Node *next;      // Pointer to the next node in case of collision

    // Constructor to initialize a Node
    Node(const string &k, const DataEntry &v) : key(k), value(v), next(nullptr) {}
};

// HashMap Class: Represents the custom hash map with separate chaining for collision handling
class HashMap
{
private:
    vector<Node *> table; // Hash table, each element points to a linked list
    int tableSize;        // Size of the hash table

    // Hash function to compute the hash value for a key
    int hashFunction(const string &key) const
    {
        // int hash = 0;
        // for (char ch : key) {
        //     hash = (hash * 31 + ch) % tableSize;
        // }
        // return hash;
        std::hash<std::string> hash; // Create a hash function object for string
        return hash(key) % tableSize;
    }

public:
    // Constructor to initialize the hash map with a given size
    HashMap(int size = 100) : tableSize(size)
    {
        table.resize(tableSize, nullptr);
    }

    // Insert method to add a key-value pair to the hash map
    void insert(const string &key, const DataEntry &value)
    {
        int index = hashFunction(key);        // Compute the hash index for the key
        Node *newNode = new Node(key, value); // Create a new node

        if (table[index] == nullptr)
        {
            table[index] = newNode; // No collision, insert directly
        }
        else
        {
            // Collision handling: separate chaining (linked list)
            Node *current = table[index];
            while (current->next != nullptr)
            {
                if (current->key == key)
                {
                    current->value = value; // Update value if key exists
                    delete newNode;         // No need to insert
                    return;
                }
                current = current->next;
            }
            current->next = newNode; // Insert at the end of the list
        }
    }

    // Get method to retrieve a value associated with a key
    bool get(const string &key, DataEntry &value) const
    {
        int index = hashFunction(key); // Compute the hash index for the key
        Node *current = table[index];

        while (current != nullptr)
        {
            if (current->key == key)
            {
                value = current->value; // Return the value if key is found
                return true;
            }
            current = current->next;
        }
        return false; // Return false if key is not found
    }
    // Method to retrieve the values (for the case of a DataEntry object)
    bool getValues(const string &key, DataEntry &value) const
    {
        return get(key, value);
    }

    // Display method to show all entries in the hash map (for testing)
    void display() const
    {
        for (int i = 0; i < tableSize; ++i)
        {
            Node *current = table[i];
            while (current != nullptr)
            {
                cout << "Key: " << current->key << endl;
                current->value.display(); // Call the display method of DataEntry
                current = current->next;
            }
        }
    }

    // Destructor to free allocated memory for linked list nodes
    ~HashMap()
    {
        for (int i = 0; i < tableSize; ++i)
        {
            Node *current = table[i];
            while (current != nullptr)
            {
                Node *temp = current;
                current = current->next;
                delete temp;
            }
        }
    }
};

// Function to limit the string to 100 words
string limitTo100Words(const string& input) {
    stringstream ss(input);
    string word;
    vector<string> words;

    // Read words into a vector
    while (ss >> word) {
        words.push_back(word);
    }

    // If there are more than 100 words, truncate the list
    if (words.size() > 20) {
        words.resize(20);
    }

    // Join the first 100 words back into a string
    stringstream result;
    for (size_t i = 0; i < words.size(); ++i) {
        result << words[i];
        if (i != words.size() - 1) {
            result << " ";  // Add space between words
        }
    }

    return result.str();
}

int hashfunction_for_doc(string key)
{
    std::hash<std::string> hash; // Create a hash function object for string
    return hash(key) % 50;
}

int main()
{
    cout<<" we are running here \n" ;
    string data_file = "six_lakh_data.json";
    // string data_file = "TheCleanData.json";
    // string data_file = "Tonelak_data.json";
    // string output_file = "link_id_map4.json";

    // Create an empty HashMap to store the data
    HashMap id_link_map1[50];
    int barel;
    ifstream datafile(data_file);
    if (!datafile.is_open())
    {
        cerr << "Could not open the data file!" << endl;
        return 1;
    }

    json data_json;
    datafile >> data_json;

    cout<<"we are running idlink5\n " ;
    vector<string> inserted_keys; // To store the inserted keys
    int counter =0 ;
    for (size_t i = 0; i < data_json.size() && i < 250000; ++i)
    {
        counter++;
        if(counter % 1000 == 0)
        {
            cout << "the data is being loaded " << counter << endl;
        }
        if (data_json[i].contains("id") && data_json[i]["id"].is_string())
        {
            string id = data_json[i]["id"].get<string>();
            inserted_keys.push_back(id); // Store the key for later retrieval
            barel = hashfunction_for_doc(id);

            DataEntry entry; // Create a new DataEntry object

            // Insert the title if available
            if (data_json[i].contains("title"))
            {
                if (data_json[i]["title"].is_string())
                {
                    entry.title = data_json[i]["title"].get<string>();
                }
                else if (data_json[i]["title"].is_array())
                {
                    for (const auto &title_item : data_json[i]["title"])
                    {
                        if (title_item.is_string())
                        {
                            entry.title = title_item.get<string>();
                        }
                    }
                }
            }

            // Insert the abstract if available
            if (data_json[i].contains("abstract") && data_json[i]["abstract"].is_string())
            {
                // entry.abstract  =  data_json[i]["abstract"].get<string>();
                string abstract_string = data_json[i]["abstract"].get<string>();
                entry.abstract = limitTo100Words(abstract_string);
            }

            // Insert the urls if available
            if (data_json[i].contains("url"))
            {
                if (data_json[i]["url"].is_array())
                {
                    for (const auto &url : data_json[i]["url"])
                    {
                        if (url.is_string())
                        {
                            entry.urls.push_back(url.get<string>());
                        }
                    }
                }
                else if (data_json[i]["url"].is_string())
                {
                    entry.urls.push_back(data_json[i]["url"].get<string>());
                }
            }

            // Insert the entry into the HashMap with the id as the key
            // id_link_map1[id] = entry;
            id_link_map1[barel].insert(id, entry);
        }
    }

    // Create a JSON object to store output
    // Create separate JSON files for each barrel
    for (int i = 0; i < 50; ++i)
    {
        json output_json;

        // For each key in the barrel, retrieve the data and add it to the output JSON
        for (const auto &key : inserted_keys)
        {
            DataEntry entry;
            if (id_link_map1[i].getValues(key, entry)) // Retrieve the DataEntry for the id
            {
                json entry_json;
                entry_json["title"] = entry.title;
                entry_json["abstract"] = entry.abstract;
                entry_json["urls"] = entry.urls;

                output_json[key] = entry_json;
            }
        }

        // Write the output to a file specific to this barrel
        string barrel_file = "./document_barels/output_barrel_" + to_string(i) + ".json";
        ofstream output(barrel_file);
        if (!output.is_open())
        {
            cerr << "Could not open the output file!" << endl;
            return 1;
        }

        output << output_json.dump(4); // Pretty print with 4 spaces
        output.close();
    }

    cout<<"the document barels are loaded here \n" ;
    return 0;
}
