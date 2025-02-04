#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>
#include <unordered_set>
#include <stdint.h> 

using namespace std;

struct CsvRow {
    string raw_line;
    string normalized;
    double time_ms;
    bool needs_trimming;
    
    bool operator<(const CsvRow& other) const {
        return time_ms < other.time_ms;
    }
};

// Returns normalized line and whether trimming occurred
pair<string, bool> normalize(const string& line) {
    stringstream ss(line);
    string segment;
    vector<string> parts;
    bool trimmed = false;
    
    while (getline(ss, segment, ';')) {
        string original = segment;
        
        // Remove quotes
        if (!segment.empty() && segment.front() == '"' && segment.back() == '"') {
            segment = segment.substr(1, segment.size() - 2);
        }
        
        // Check and trim whitespace
        size_t first = segment.find_first_not_of(" \t");
        size_t last = segment.find_last_not_of(" \t");
        if (first != string::npos) {
            string trimmed_seg = segment.substr(first, (last - first + 1));
            if (trimmed_seg != segment) {
                trimmed = true;
            }
            segment = trimmed_seg;
        } else {
            segment.clear();
        }
        
        if (!segment.empty()) {
            parts.push_back(segment);
        }
    }
    
    string result;
    for (size_t i = 0; i < parts.size(); ++i) {
        result += parts[i];
        if (i != parts.size() - 1) result += ";";
    }
    
    return {result, trimmed};
}

vector<CsvRow> read_and_prepare(const string& filename, vector<pair<string, string>>& trimmed_lines) {
    vector<CsvRow> rows;
    ifstream file(filename);
    string line;
    
    getline(file, line); // Skip header
    while (getline(file, line)) {
        auto [normalized, was_trimmed] = normalize(line);
        size_t first_semi = line.find(';');
        double timestamp = stod(line.substr(0, first_semi));
        
        rows.push_back({line, normalized, timestamp, was_trimmed});
        
        if (was_trimmed) {
            trimmed_lines.emplace_back(line, normalized);
        }
    }
    
    return rows;
}

int main() {

    uint16_t myCount = 0;

    string file1, file2, output;
    cout << "Enter first CSV file: ";
    getline(cin, file1);
    cout << "Enter second CSV file: ";
    getline(cin, file2);
    cout << "Enter output file name: ";
    getline(cin, output);

    // Track trimmed lines
    vector<pair<string, string>> trimmed_lines;

    // Read and combine data
    auto data1 = read_and_prepare(file1, trimmed_lines);
    auto data2 = read_and_prepare(file2, trimmed_lines);
    vector<CsvRow> all_data;
    all_data.reserve(data1.size() + data2.size());
    all_data.insert(all_data.end(), data1.begin(), data1.end());
    all_data.insert(all_data.end(), data2.begin(), data2.end());

    // Sort by timestamp
    sort(all_data.begin(), all_data.end());

    // Deduplicate
    vector<CsvRow> unique_data;
    unordered_set<string> seen;
    for (const auto& row : all_data) {
        if (seen.insert(row.normalized).second) {
            unique_data.push_back(row);
        }
    }

    // Write output
    ofstream out(output);
    out << "\"Time_ms\";\"MsgProc\";\"StateAfter\";\"MsgClass\";\"MsgNumber\";"
        << "\"Var1\";\"Var2\";\"Var3\";\"Var4\";\"Var5\";\"Var6\";\"Var7\";"
        << "\"Var8\";\"TimeString\";\"MsgText\";\"PLC\"\n";
    
    for (const auto& row : unique_data) {
        out << row.raw_line << "\n";
    }

    // Print trimming report
    if (!trimmed_lines.empty()) {
        cout << "\nWhitespace trimming occurred in " << trimmed_lines.size() << " rows:\n";
        for (const auto& [original, trimmed] : trimmed_lines) {
            myCount = myCount +1;
            cout << "Original: " << original << "\n";
            cout << "Trimmed:  " << trimmed << "\n\n";
        }
    } else {
        cout << "\nNo whitespace trimming was needed.\n";
    }

    cout << "\nSorted and deduplicated file created: " << output << endl;
    cout << "\nHow many were whitespace trimmed: " << myCount << endl;
    return 0;
}