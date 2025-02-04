#include <iostream>
#include <fstream>
#include <string>
#include <unordered_set>
#include <sstream>
#include <vector>

using namespace std;

string normalize(const string& line) {
    stringstream ss(line);
    string segment;
    vector<string> parts;
    
    while (getline(ss, segment, ';')) {
        // Remove quotes
        if (segment.size() >= 2 && segment.front() == '"' && segment.back() == '"') {
            segment = segment.substr(1, segment.size() - 2);
        }
        
        // Trim whitespace
        segment.erase(0, segment.find_first_not_of(" \t"));
        segment.erase(segment.find_last_not_of(" \t") + 1);
        
        if (!segment.empty()) {
            parts.push_back(segment);
        }
    }
    
    stringstream result;
    for (size_t i = 0; i < parts.size(); ++i) {
        result << parts[i];
        if (i != parts.size() - 1) result << ";";
    }
    return result.str();
}

int count_duplicates(const string& file1, const string& file2) {
    unordered_set<string> file1_data;
    int duplicate_count = 0;

    // Read first file
    ifstream f1(file1);
    if (!f1.is_open()) {
        cerr << "Error opening: " << file1 << endl;
        return -1;
    }
    
    string line;
    getline(f1, line); // Skip header
    while (getline(f1, line)) {
        file1_data.insert(normalize(line));
    }
    f1.close();

    // Check second file
    ifstream f2(file2);
    if (!f2.is_open()) {
        cerr << "Error opening: " << file2 << endl;
        return -1;
    }
    
    getline(f2, line); // Skip header
    while (getline(f2, line)) {
        string normalized = normalize(line);
        if (file1_data.count(normalized)) {
            duplicate_count++;
        }
    }
    f2.close();

    return duplicate_count;
}

int main() {
    string file1, file2;
    
    cout << "Enter first CSV filename: ";
    getline(cin, file1);
    
    cout << "Enter second CSV filename: ";
    getline(cin, file2);

    int result = count_duplicates(file1, file2);
    
    if (result >= 0) {
        cout << "Total duplicate rows: " << result << endl;
    } else {
        cout << "Error processing files" << endl;
    }

    return 0;
}