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
