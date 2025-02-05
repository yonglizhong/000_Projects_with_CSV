#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>
#include <unordered_set>
#include <chrono>
#include <conio.h>

using namespace std;

struct CsvRow {
    string raw_line;
    double time_ms;
    
    bool operator<(const CsvRow& other) const {
        return time_ms < other.time_ms;
    }
};

const map<int, string> PATH_OPTIONS = {
    {1, "P:/Fehler Historie & Wartungsplan der Anlagen/1_AG1A/2024/"},
    {2, "P:/Fehler Historie & Wartungsplan der Anlagen/2_SL1B/2024/"},
    {3, "P:/Fehler Historie & Wartungsplan der Anlagen/3_SL1A/2024/"},
    {4, "P:/Fehler Historie & Wartungsplan der Anlagen/4_SEED1A/2024/"},
    {5, "P:/Fehler Historie & Wartungsplan der Anlagen/5_DECK1A/2024/"},
    {6, "P:/Fehler Historie & Wartungsplan der Anlagen/6_ISD1A/2024/"}
};

string get_full_path() {
    int choice;
    cout << "Choose a file path (1-6): ";
    cin >> choice;
    cin.ignore();
    
    if (PATH_OPTIONS.find(choice) != PATH_OPTIONS.end()) {
        return PATH_OPTIONS.at(choice);
    }
    
    cout << "Invalid choice. Using current directory.\n";
    return "";
}

vector<CsvRow> read_and_prepare(const string& filename) {
    vector<CsvRow> rows;
    ifstream file(filename);
    if (!file) {
        cerr << "Error opening file: " << filename << "\n";
        return rows;
    }
    string line;
    getline(file, line);
    while (getline(file, line)) {
        size_t first_semi = line.find(';');
        double timestamp = stod(line.substr(0, first_semi));
        rows.push_back({line, timestamp});
    }
    return rows;
}

int main() {
    do {
        auto start_time = chrono::high_resolution_clock::now();
        
        string base_path = get_full_path();
        
        string file1, file2, output;
        cout << "Enter first CSV file name: ";
        getline(cin, file1);
        file1 = base_path + file1;
        
        cout << "Enter second CSV file name: ";
        getline(cin, file2);
        file2 = base_path + file2;
        
        cout << "Enter output file name: ";
        getline(cin, output);
        output = base_path + output;
        
        auto data1 = read_and_prepare(file1);
        auto data2 = read_and_prepare(file2);
        
        vector<CsvRow> all_data;
        all_data.reserve(data1.size() + data2.size());
        all_data.insert(all_data.end(), data1.begin(), data1.end());
        all_data.insert(all_data.end(), data2.begin(), data2.end());
        
        sort(all_data.begin(), all_data.end());
        unordered_set<string> seen;
        vector<CsvRow> unique_data;
        int duplicate_count = 0;
        for (const auto& row : all_data) {
            if (seen.insert(row.raw_line).second) {
                unique_data.push_back(row);
            } else {
                duplicate_count++;
            }
        }
        
        ofstream out(output);
        out << "\"Time_ms\";\"MsgProc\";\"StateAfter\";\"MsgClass\";\"MsgNumber\";"
            "\"Var1\";\"Var2\";\"Var3\";\"Var4\";\"Var5\";\"Var6\";\"Var7\";"
            "\"Var8\";\"TimeString\";\"MsgText\";\"PLC\"\n";
        for (const auto& row : unique_data) {
            out << row.raw_line << "\n";
        }
        
        cout << "\nNumber of duplicate lines: " << duplicate_count << "\n";
        
        auto end_time = chrono::high_resolution_clock::now();
        chrono::duration<double> duration = end_time - start_time;
        cout << "\nExecution Time: " << duration.count() << " seconds\n";
        
        cout << "\nPress ESC to exit or any other key to process another file set...";
    } while (_getch() != 27);
    return 0;
}
