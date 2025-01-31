#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <mutex>
#include <thread>

using namespace std;

class ThreadSafeStorage {
    unordered_map<double, vector<string>> data; // Key: numerical Time_ms
    mutex mtx;

public:
    void insert(const string& line) {
        size_t pos = line.find(';');
        if(pos == string::npos) return;

        double time_ms = stod(line.substr(0, pos));
        
        lock_guard<mutex> lock(mtx);
        if(find(data[time_ms].begin(), data[time_ms].end(), line) == data[time_ms].end()) {
            data[time_ms].push_back(line);
        }
    }

    void write_sorted(const string& output_file, const string& header) {
        vector<double> keys;
        for(const auto& pair : data) {
            keys.push_back(pair.first);
        }
        sort(keys.begin(), keys.end());

        ofstream out(output_file);
        out << header << "\n";
        
        for(double key : keys) {
            for(const auto& line : data[key]) {
                out << line << "\n";
            }
        }
    }
};

void process_file(const string& filename, ThreadSafeStorage& storage) {
    ifstream file(filename);
    string line;
    getline(file, line); // Skip header
    
    while(getline(file, line)) {
        storage.insert(line);
    }
    file.close();
}

int main() {
    const vector<string> files = {"test1.csv", "test2.csv"};
    const string output_file = "combined_sorted.csv";
    ThreadSafeStorage storage;

    // Get header from first file
    ifstream header_stream(files[0]);
    string header;
    getline(header_stream, header);
    header_stream.close();

    // Process files in parallel
    vector<thread> threads;
    for(const auto& file : files) {
        threads.emplace_back(process_file, file, ref(storage));
    }

    for(auto& t : threads) {
        t.join();
    }

    // Write sorted output
    storage.write_sorted(output_file, header);
    cout << "Sorted combined file created: " << output_file << endl;

    return 0;
}
