#include <iostream>
#include <nlohmann/json.hpp>
#include <fstream>
#include <vector>
#include <sstream>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <string>
#include <mutex>
#include <functional>
#include <regex> // For regular expression handling

using json = nlohmann::json;
using namespace std;

class VectorOfString
{
private:
    string *data;        // Pointer to dynamically allocated array to store strings
    size_t current_size; // Current number of elements in the array
    size_t capacity;     // Maximum capacity before needing a resize

    // Resize the internal array when capacity is reached
    void resize()
    {
        // Double the capacity
        capacity = (capacity == 0) ? 1 : capacity * 2;
        string *new_data = new string[capacity]; // Allocate new array

        // Copy the current elements to the new array
        for (size_t i = 0; i < current_size; ++i)
        {
            new_data[i] = data[i];
        }

        // Delete old data array and point to new one
        delete[] data;
        data = new_data;
    }

public:
    // Constructor to initialize the dynamic array
    VectorOfString() : data(nullptr), current_size(0), capacity(0) {}

    // Destructor to free the dynamically allocated memory
    ~VectorOfString()
    {
        delete[] data;
    }

    // Push a string to the vector (array)
    void push_back(const string &str)
    {
        if (current_size == capacity)
        {
            resize(); // Resize if capacity is full
        }
        data[current_size] = str;
        ++current_size;
    }

    // Get the number of elements in the array
    size_t size() const
    {
        return current_size;
    }

    // Access an element by index
    string &operator[](size_t index)
    {
        if (index < current_size)
        {
            return data[index];
        }
        throw out_of_range("Index out of range");
    }
    // Define a method to convert VectorOfString to JSON
    json to_json() const
    {
        json j = json::array(); // Create an empty JSON array
        for (size_t i = 0; i < current_size; ++i)
        {
            j.push_back(data[i]); // Add each string to the JSON array
        }
        return j;
    }

    string *begin()
    {
        return data;
    }

    string *end()
    {
        return data + current_size;
    }

    // Print all elements in the array
    void print() const
    {
        for (size_t i = 0; i < current_size; ++i)
        {
            cout << data[i] << endl;
        }
    }
};

// Node for the linked list in each bucket

// Node class for linked list nodes
class Node
{
private:
    string data;
    Node *next;

public:
    explicit Node(const string &data) : data(data), next(nullptr) {}

    const string &getData() const
    {
        return data;
    }

    void setNext(Node *nextNode)
    {
        next = nextNode;
    }

    Node *getNext() const
    {
        return next;
    }
};

// Custom Vector class for dynamic array
class Vector
{
private:
    Node **data;     // Dynamic array of Node pointers
    size_t capacity; // Allocated memory
    size_t size;     // Current number of elements

    // Reallocate memory with a new capacity
    void reallocate(size_t new_capacity)
    {
        Node **new_data = new Node *[new_capacity];
        for (size_t i = 0; i < size; ++i)
        {
            new_data[i] = data[i];
        }
        delete[] data;
        data = new_data;
        capacity = new_capacity;
    }

public:
    // Constructor
    Vector(size_t initial_capacity = 4)
        : data(new Node *[initial_capacity]), capacity(initial_capacity), size(0) {}

    // Destructor
    ~Vector()
    {
        delete[] data;
    }

    // Access element by index (non-const version)
    Node *&operator[](size_t index)
    {
        return data[index]; // Allows modification
    }

    // Access element by index (const version)
    const Node *operator[](size_t index) const
    {
        return data[index]; // Prevent modification
    }

    // Resize the vector
    void resize(size_t new_size)
    {
        if (new_size > capacity)
        {
            reallocate(new_size);
        }
        for (size_t i = size; i < new_size; ++i)
        {
            data[i] = nullptr; // Initialize new elements to nullptr
        }
        size = new_size;
    }

    // Move semantics: Move constructor
    Vector(Vector &&other) noexcept
        : data(other.data), capacity(other.capacity), size(other.size)
    {
        other.data = nullptr;
        other.capacity = 0;
        other.size = 0;
    }

    // Move assignment operator
    Vector &operator=(Vector &&other) noexcept
    {
        if (this != &other)
        {
            delete[] data;
            data = other.data;
            capacity = other.capacity;
            size = other.size;

            other.data = nullptr;
            other.capacity = 0;
            other.size = 0;
        }
        return *this;
    }

    // Clear the vector
    void clear()
    {
        size = 0;
    }

    // Get the current size of the vector
    size_t getSize() const
    {
        return size;
    }
};

// Custom iterator for linked list traversal
class Iterator
{
private:
    Node *current;

public:
    explicit Iterator(Node *node) : current(node) {}

    const string &operator*() const
    {
        return current->getData();
    }

    bool operator==(const Iterator &other) const
    {
        return current == other.current;
    }

    bool operator!=(const Iterator &other) const
    {
        return !(*this == other);
    }

    Iterator &operator++()
    {
        if (current)
        {
            current = current->getNext();
        }
        return *this;
    }
};

// CustomSet implementation
class CustomSet
{
private:
    static const size_t DEFAULT_BUCKET_COUNT = 16;
    Vector buckets;      // Use custom Vector for buckets
    size_t bucket_count; // Number of buckets
    size_t current_size; // Number of elements in the set

    // Hash function
    size_t hash(const string &key) const
    {
        std::hash<string> hasher;
        return hasher(key) % bucket_count;
    }

    // Resize the bucket array
    void resize()
    {
        size_t new_bucket_count = bucket_count * 2;
        Vector new_buckets;
        new_buckets.resize(new_bucket_count);

        // Rehash all elements and move them to the new buckets
        for (size_t i = 0; i < bucket_count; ++i)
        {
            Node *current = buckets[i]; // Non-const access to bucket_head
            while (current)
            {
                size_t new_index = std::hash<string>{}(current->getData()) % new_bucket_count;
                Node *temp = current->getNext();
                current->setNext(new_buckets[new_index]);
                new_buckets[new_index] = current;
                current = temp;
            }
        }

        bucket_count = new_bucket_count;
        buckets = std::move(new_buckets);
    }

public:
    // Constructor
    CustomSet()
        : bucket_count(DEFAULT_BUCKET_COUNT), current_size(0), buckets()
    {
        buckets.resize(DEFAULT_BUCKET_COUNT);
    }

    // Insert function
    void insert(const string &key)
    {
        if (current_size >= bucket_count * 0.75)
        {
            resize();
        }

        size_t bucket_index = hash(key);
        Node *&bucket_head = buckets[bucket_index]; // Non-const reference
        Node *current = bucket_head;

        // Check for duplicates
        while (current)
        {
            if (current->getData() == key)
            {
                return; // Already exists
            }
            current = current->getNext();
        }

        // Insert at the beginning of the bucket's linked list
        Node *newNode = new Node(key);
        newNode->setNext(bucket_head);
        bucket_head = newNode;
        current_size++;
    }

    // Convert CustomSet to nlohmann::json
    nlohmann::json toJson() const
    {
        nlohmann::json json_array;

        for (size_t i = 0; i < bucket_count; ++i)
        {
            const Node *current = buckets[i];
            while (current)
            {
                json_array.push_back(current->getData()); // Adding elements to the JSON array
                current = current->getNext();
            }
        }

        return json_array;
    }

    // Find function
    Iterator find(const string &key) const
    {
        size_t bucket_index = hash(key);
        const Node *current = buckets[bucket_index]; // Const access to bucket_head

        while (current)
        {
            if (current->getData() == key)
            {
                return Iterator(const_cast<Node *>(current)); // Casting const to non-const for Iterator
            }
            current = current->getNext();
        }
        return end();
    }

    // End function
    Iterator end() const
    {
        return Iterator(nullptr);
    }

    // Provide begin and end methods to make the CustomSet iterable in a range-based for loop

    // Destructor
    ~CustomSet()
    {
        for (size_t i = 0; i < bucket_count; ++i)
        {
            Node *bucket = buckets[i];
            while (bucket)
            {
                Node *temp = bucket;
                bucket = bucket->getNext();
                delete temp;
            }
        }
    }
};

// Helper function to split a string into words (for abstract processing)

// VectorOfString split_into_words(const string& str) {
//     VectorOfString words;
//     stringstream ss(str);
//     string word;

//     // Split string into words by spaces
//     while (ss >> word) {
//         // Remove any leading/trailing spaces and make it lowercase for consistent comparison
//         transform(word.begin(), word.end(), word.begin(), ::tolower); // Convert to lowercase
//         words.push_back(word);
//     }

//     return words;
// }
VectorOfString split_into_words(const std::string &str)
{
    VectorOfString words;

    std::regex wordRegex(R"([a-zA-Z0-9]+)");

    // Use regex_iterator to extract all individual words
    std::sregex_iterator wordsBegin(str.begin(), str.end(), wordRegex);
    std::sregex_iterator wordsEnd;

    for (std::sregex_iterator it = wordsBegin; it != wordsEnd; ++it) {
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
    // Use regex to remove any character that is not a letter (a-z, A-Z)
    string cleaned_word = word;
    // cleaned_word = regex_replace(cleaned_word, regex("[^a-zA-Z]"), "");  // Remove non-alphabetic characters
    return cleaned_word;
}

// A function to lemmatize a word by removing common verb and plural noun suffixes
string lemmatize_word(const string &word)
{
    string w = word;
    transform(w.begin(), w.end(), w.begin(), ::tolower); // Convert to lowercase for consistency

    // Dictionary for irregular verbs (example: "ate" → "eat")
    static unordered_map<string, string> irregular_verbs = {
        {"ate", "eat"},
        {"went", "go"},
        {"had", "have"},
        {"did", "do"},
        {"was", "be"},
        {"were", "be"},
        // Add more irregular verbs as needed
    };

    // Step 1: Handle irregular verbs first (simple lookup in the dictionary)
    if (irregular_verbs.find(w) != irregular_verbs.end())
    {
        return irregular_verbs[w]; // Return the base form of the irregular verb
    }

    // Step 2: Handle regular verbs (present participle, past tense, third-person singular)
    if (w.length() > 3)
    {
        // Rule 1: Remove "ing" (present participle) - special case for verbs like "leaving" → "leave"
        if (w.substr(w.length() - 3) == "ing")
        {
            // Special case handling for verbs like "leaving" → "leave"
            // Check if removing "ing" results in a valid root word (not too short)
            string root = w.substr(0, w.length() - 3);
            if (root.length() > 2)
            {
                return root;
            }
        }

        // Rule 2: Remove "ed" (past tense)
        if (w.substr(w.length() - 2) == "ed")
        {
            return w.substr(0, w.length() - 2); // e.g., "played" → "play"
        }

        // Rule 3: Remove "es" (third-person singular)
        if (w.substr(w.length() - 2) == "es")
        {
            return w.substr(0, w.length() - 2); // e.g., "runs" → "run"
        }
    }

    // Step 3: Handle plural nouns (remove "s" or "es")
    if (w.length() > 2)
    {
        // Rule 4: Remove "s" or "es" for plural nouns
        if (w.substr(w.length() - 1) == "s")
        {
            if (w.length() > 3 && w.substr(w.length() - 2) == "es")
            {
                return w.substr(0, w.length() - 2); // e.g., "boxes" → "box"
            }
            return w.substr(0, w.length() - 1); // e.g., "cats" → "cat"
        }
    }

    return w; // Return the original word if no rule applies
}

int main()
{
    // Load stop words from stopword.json file
    // CustomSet stop_words = load_stop_words("combined_stopwords.json");
    // unordered_set<string> stop_words = load_stop_words("combined_stopwords.json");

    CustomSet stop_words;

    // Open and parse the JSON file
    ifstream file1("combined_stopwords.json");
    if (!file1.is_open())
    {
        cerr << "Could not open the stopword file!" << endl;
        return 1;
    }

    json k;
    file1 >> k;

    // Assuming stopwords are in a JSON array
    if (k.is_object() && k.contains("stopwords") && k["stopwords"].is_array())
    {
        for (const auto &word : k["stopwords"])
        {
            if (word.is_string())
            {

                string stop_word = word.get<string>();
                // stop_word = clean_word(stop_word); // Clean the word
                stop_words.insert(stop_word); // Insert into CustomSet
            }
        }
    }
    else
    {
        cerr << "Error: The JSON file does not contain the expected structure." << endl;
        return 1;
    }

    // Print the stop words that were loaded
    cout << "Loaded Stop Words:" << endl;
    // for (const string& word : stop_words) {
    //     cout << word << endl;
    // }

    // Open the JSON file containing the data
    ifstream file("Tonelak_data.json");
    if (!file.is_open())
    {
        cerr << "Could not open the file!" << endl;
        return 1;
    }

    // Parse the JSON data from the file
    json j;
    file >> j;

    // Vector to store all the data in one place
    VectorOfString basra;
    // VectorOfString basra;

    // Set to track unique words (prevents duplicates)
    CustomSet unique_words;
    // unordered_set<string> unique_words;
    int stepper = 1;

    // Loop through the first 10 objects (or less if the file has fewer objects)
    for (size_t i = 0; i < 1000 && i < j.size(); ++i)
    {
        cout << stepper << endl;
        stepper++;
        // Extract title and add to basra
        if (j[i].contains("title"))
        {
            if (j[i]["title"].is_string())
            {
                string title = j[i]["title"].get<string>();
                VectorOfString words = split_into_words(title);
                for (string title : words)
                {
                    title = clean_word(title); // Clean the title by removing non-alphabetical characters
                    if (stop_words.find(title) == stop_words.end())
                    {                                  // Check for stop word
                        title = lemmatize_word(title); // Apply lemmatization
                        if (unique_words.find(title) == unique_words.end())
                        {
                            // basra.push_back("Title:");
                            basra.push_back(title);
                            unique_words.insert(title); // Mark this word as used
                        }
                    }
                }
            }
            else if (j[i]["title"].is_array())
            {
                for (const auto &title_item : j[i]["title"])
                {
                    if (title_item.is_string())
                    {
                        string title = title_item.get<string>();
                        VectorOfString words = split_into_words(title);
                        for (string title : words)
                        {
                            title = clean_word(title); // Clean the title by removing non-alphabetical characters
                            if (stop_words.find(title) == stop_words.end())
                            {                                  // Check for stop word
                                title = lemmatize_word(title); // Apply lemmatization
                                if (unique_words.find(title) == unique_words.end())
                                {
                                    // basra.push_back("Title:");
                                    basra.push_back(title);
                                    unique_words.insert(title); // Mark this word as used
                                }
                            }
                        }
                    }
                }
            }
        }

        // Extract abstract and split into words, then add to basra
        if (j[i].contains("abstract"))
        {
            if (j[i]["abstract"].is_string())
            {
                VectorOfString words = split_into_words(j[i]["abstract"]);
                // basra.push_back("Abstract Words:");
                for (string word : words)
                {
                    word = clean_word(word); // Clean each word
                    if (stop_words.find(word) == stop_words.end())
                    {                                // Check for stop word
                        word = lemmatize_word(word); // Apply lemmatization only for non-stop words
                        if (unique_words.find(word) == unique_words.end())
                        {
                            basra.push_back(word);
                            unique_words.insert(word); // Mark this word as used
                        }
                    }
                }
            }
        }

        // Extract keywords and add each keyword separately
        if (j[i].contains("keywords"))
        {
            // basra.push_back("Keywords:");
            if (j[i]["keywords"].is_array())
            {
                for (const auto &keyword : j[i]["keywords"])
                {
                    if (keyword.is_string())
                    {
                        VectorOfString keyword_parts = split_into_words(keyword.get<string>());
                        for (string part : keyword_parts)
                        {
                            part = clean_word(part); // Clean each keyword part
                            if (stop_words.find(part) == stop_words.end())
                            {                                // Check for stop word
                                part = lemmatize_word(part); // Apply lemmatization only for non-stop words
                                if (unique_words.find(part) == unique_words.end())
                                {
                                    basra.push_back(part);
                                    unique_words.insert(part); // Mark this word as used
                                }
                            }
                        }
                    }
                }
            }
        }

        // Extract year and handle both cases where it's a string or a number, then add to basra
        if (j[i].contains("year"))
        {
            string year;
            if (j[i]["year"].is_number())
            {
                year = to_string(j[i]["year"].get<int>());
            }
            else if (j[i]["year"].is_string())
            {
                year = j[i]["year"].get<string>();
            }

            if (unique_words.find(year) == unique_words.end())
            {
                // basra.push_back("Year:");
                basra.push_back(year);
                unique_words.insert(year); // Mark this word as used
            }
        }

        // Extract author names and add each name separately
        if (j[i].contains("authors") && !j[i]["authors"].empty())
        {
            // basra.push_back("Authors:");
            for (const auto &author : j[i]["authors"])
            {
                if (author["name"].is_string())
                {
                    VectorOfString author_parts = split_into_words(author["name"].get<string>());
                    for (string part : author_parts)
                    {                            // Create a copy of 'part' to modify it
                        part = clean_word(part); // Clean each author part
                        if (stop_words.find(part) == stop_words.end())
                        {                                // Check for stop word
                            part = lemmatize_word(part); // Apply lemmatization only for non-stop words
                            if (unique_words.find(part) == unique_words.end())
                            {
                                basra.push_back(part);
                                unique_words.insert(part); // Mark this word as used
                            }
                        }
                    }
                }
            }
        }
    }

    // Debugging: print out what we have before saving to JSON
    cout << "\nAll Data in 'lexicon' after processing:" << endl;
    // for (const string& data : basra) {
    //     cout << data << endl;
    // }

    // Convert basra vector to JSON and save it to the file
    ofstream output_file("lexicon.json");
    json output_json = basra.to_json();
    output_file << output_json.dump(4); // Pretty print with an indent of 4 spaces
    output_file.close();

    return 0;
}
