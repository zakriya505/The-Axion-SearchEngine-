#include <iostream>
#include <fstream>
#include <functional> // For std::hash
#include <vector>
#include <string>
#include "nlohmann/json.hpp"

using json = nlohmann::json;
using namespace std;

const int HASH_SIZE = 1009; // Prime number for hash table size

class Document
{
public:
    string docID;
    vector<string> words; // List of words present in the document

    Document(const string &id) : docID(id) {}
    void addWord(const string &word)
    {
        words.push_back(word);
    }
};

class WordFrequency
{
public:
    string word;
    vector<Document> documents; // List of documents that contain the word

    WordFrequency(const string &w) : word(w) {}

    void addDocument(const string &docID)
    {
        for (auto &doc : documents)
        {
            if (doc.docID == docID)
            {
                return; // Document already added, no need to insert again
            }
        }
        documents.push_back(Document(docID));
    }
};

class HashMap
{
public:
    struct Node
    {
        string key;
        WordFrequency *value;
        Node *next;

        Node(const string &k) : key(k), value(nullptr), next(nullptr) {}
    };

    Node *table[HASH_SIZE]; // Array of pointers to nodes

    // Hash function to map strings to an index in the table

    // Hash function to map strings to an index in the table using std::hash
    int hash(const string &key) const
    {
        std::hash<std::string> hash_fn;  // Create a hash function object
        size_t hashValue = hash_fn(key); // Get the hash value of the string
        return hashValue % HASH_SIZE;    // Map it to the table size (HASH_SIZE)
    }

public:
    HashMap()
    {
        fill(begin(table), end(table), nullptr); // Initialize all buckets to nullptr
    }

    // Insert or update a key with a value
    void insert(const string &key, const string &docID)
    {
        int index = hash(key);
        Node *node = table[index];

        // Check if the key already exists, if so, just add the document ID
        while (node != nullptr)
        {
            if (node->key == key)
            {
                if (!node->value)
                {
                    node->value = new WordFrequency(key);
                }
                node->value->addDocument(docID);
                return;
            }
            node = node->next;
        }

        // If the key doesn't exist, create a new node and insert it
        Node *newNode = new Node(key);
        newNode->value = new WordFrequency(key);
        newNode->value->addDocument(docID);
        newNode->next = table[index];
        table[index] = newNode;
    }

    // Retrieve the WordFrequency object for a word
    WordFrequency *get(const string &key) const
    {
        int index = hash(key);
        Node *node = table[index];

        while (node != nullptr)
        {
            if (node->key == key)
            {
                return node->value; // Return the WordFrequency object
            }
            node = node->next;
        }
        return nullptr; // Return nullptr if the key doesn't exist
    }

    // Destructor to free allocated memory
    ~HashMap()
    {
        for (int i = 0; i < HASH_SIZE; ++i)
        {
            Node *node = table[i];
            while (node != nullptr)
            {
                Node *toDelete = node;
                node = node->next;
                delete toDelete->value; // Delete associated WordFrequency object
                delete toDelete;
            }
        }
    }
};

class InvertedIndex
{
public:
    HashMap invertedMap; // HashMap for word -> WordFrequency

    void addWordToInvertedIndex(const string &word, const string &docID)
    {
        invertedMap.insert(word, docID);
    }

    void saveToJson(const string &outputFilePath)
    {
        const int NUM_BUCKETS = 50;                     // Number of barrels/buckets
        vector<json> invertedIndexBuckets(NUM_BUCKETS); // Create 50 JSON objects for the barrels

        // Iterate through the hash map and distribute keys into barrels
        for (int i = 0; i < HASH_SIZE; ++i)
        {
            HashMap::Node *node = invertedMap.table[i];
            while (node != nullptr)
            {
                // Calculate the bucket index using hash(key) % 50
                size_t hashValue = hash<string>{}(node->key);
                int bucketIndex = hashValue % NUM_BUCKETS;

                // Add key and its document list to the appropriate barrel
                json docIDs;
                for (auto &doc : node->value->documents)
                {
                    docIDs.push_back(doc.docID);
                }
                invertedIndexBuckets[bucketIndex][node->key] = docIDs;

                node = node->next;
            }
        }

        // Write each barrel's JSON data to a separate file
        for (int i = 0; i < NUM_BUCKETS; ++i)
        {
            string fileName = outputFilePath + "/inverted_index_bucket_" + to_string(i) + ".json";
            ofstream outFile(fileName);
            if (!outFile.is_open())
            {
                cerr << "Error: Could not open file " << fileName << " for writing." << endl;
                continue;
            }

            outFile << invertedIndexBuckets[i].dump(4); // Write with indentation for readability
            outFile.close();

            cout << "Inverted index for barrel " << i << " has been saved to " << fileName << endl;
        }
    }
};

int main()
{
    // File paths for the lexicon and forward index JSON files
    string lexiconFilePath = "lexicon.json";
    string forwardIndexFilePath = "forward_index256.json";
    string outputFilePath = "./inverted_index_barels"; // Output file path

    // Read and parse the lexicon JSON file
    ifstream lexiconFile(lexiconFilePath);
    if (!lexiconFile.is_open())
    {
        cerr << "Error: Could not open lexicon file." << endl;
        return 1;
    }
    json lexiconJson;
    lexiconFile >> lexiconJson;
    lexiconFile.close();
    vector<string> lexicon = lexiconJson.get<vector<string>>();

    // Read and parse the forward index JSON file
    ifstream forwardIndexFile(forwardIndexFilePath);
    if (!forwardIndexFile.is_open())
    {
        cerr << "Error: Could not open forward index file." << endl;
        return 1;
    }
    json forwardIndexJson;
    forwardIndexFile >> forwardIndexJson;
    forwardIndexFile.close();

    // Create the inverted index
    InvertedIndex invertedIndex;

    // Iterate through each word in the lexicon
    for (const string &word : lexicon)
    {
        // Check each document in the forward index
        for (auto &[docID, wordFreqMap] : forwardIndexJson.items())
        {
            if (wordFreqMap.contains(word))
            {
                // If the word is found in this document, add the document ID to the inverted index
                invertedIndex.addWordToInvertedIndex(word, docID);
            }
        }
    }

    // Save the inverted index to a JSON file
    invertedIndex.saveToJson(outputFilePath);

    return 0;
}
