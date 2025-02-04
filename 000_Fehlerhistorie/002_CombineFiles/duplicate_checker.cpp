#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <algorithm>

using namespace std;

// Normalization function remains unchanged
string normalize(const string& line) {
    stringstream ss(line);
    string segment;
    vector<string> parts;
    
    while (getline(ss, segment, ';')) {
        // Remove surrounding quotes
        if (segment.size() >= 2 && segment.front() == '"' && segment.back() == '"') {
            segment = segment.substr(1, segment.size() - 2);
        }
        
        // Trim whitespace from both ends
        segment.erase(0, segment.find_first_not_of(" \t"));
        segment.erase(segment.find_last_not_of(" \t") + 1);
        
        if (!segment.empty()) {
            parts.push_back(segment);
        }
    }
    
    // Rebuild normalized line
    string result;
    for (size_t i = 0; i < parts.size(); ++i) {
        result += parts[i];
        if (i != parts.size() - 1)
            result += ";";
    }
    return result;
}

// Function to print duplicate rows along with their line numbers.
void printDuplicateDetails(const unordered_map<string, vector<int>>& rowPositions) {
    cout << "\nDuplicated rows found:\n";
    bool duplicatesFound = false;
    for (const auto& [row, positions] : rowPositions) {
        if (positions.size() > 1) { // More than one occurrence
            duplicatesFound = true;
            cout << "Row: \"" << row << "\" appears on line numbers: ";
            // Print positions separated by commas.
            for (size_t i = 0; i < positions.size(); ++i) {
                cout << positions[i];
                if (i != positions.size() - 1)
                    cout << ", ";
            }
            cout << "\n";
        }
    }
    
    if (!duplicatesFound) {
        cout << "No duplicate rows were found.\n";
    }
}

int main() {
    string filename;
    cout << "Enter CSV filename to check for duplicates: ";
    getline(cin, filename);

    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open file '" << filename << "'" << endl;
        return 1;
    }

    // Map from normalized row to the count of occurrences
    unordered_map<string, int> rowCounts;
    // Map from normalized row to a vector of line numbers (excluding header)
    unordered_map<string, vector<int>> rowPositions;

    string line;
    int total_duplicates = 0;
    int total_lines = 0;
    
    // Skip header (and optionally store it if needed)
    getline(file, line);
    
    // Process the file lines
    while (getline(file, line)) {
        total_lines++;
        string normalized = normalize(line);
        rowCounts[normalized]++;
        rowPositions[normalized].push_back(total_lines);  // record the line number (excluding header)
    }
    file.close();

    // Calculate total duplicate instances
    for (const auto& [row, count] : rowCounts) {
        if (count > 1) {
            total_duplicates += (count - 1);
        }
    }

    cout << "\nFile analysis results:\n";
    cout << "Total lines processed: " << total_lines << endl;
    cout << "Unique rows found:     " << rowCounts.size() << endl;
    cout << "Duplicate instances:   " << total_duplicates << endl;

    // Print out details for each duplicate row
    printDuplicateDetails(rowPositions);

    return 0;
}
