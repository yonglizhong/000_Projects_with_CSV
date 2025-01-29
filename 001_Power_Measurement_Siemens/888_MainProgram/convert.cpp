#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <ctime>
#include <algorithm>

// Function to convert timestamp to seconds relative to base time
int convertToSeconds(const std::string& timestamp, const std::tm& base_time) {
    std::tm time_struct = {};
    std::istringstream ss(timestamp);
    ss >> std::get_time(&time_struct, "%d.%m.%Y %H:%M:%S");
    if (ss.fail()) {
        throw std::runtime_error("Failed to parse timestamp: " + timestamp);
    }
    return std::difftime(std::mktime(&time_struct), std::mktime(const_cast<std::tm*>(&base_time)));
}

// Function to replace all occurrences of a character in a string
void replaceChar(std::string& str, char oldChar, char newChar) {
    std::replace(str.begin(), str.end(), oldChar, newChar);
}

// Function to process power values and convert units
std::string processValue(const std::string& value) {
    if (value.empty()) return "";

    size_t pos = value.find(' ');
    std::string num_str = value.substr(0, pos);
    std::string unit = value.substr(pos + 1);

    replaceChar(num_str, ',', '.'); // Replace ',' with '.'
    double num = std::stod(num_str);

    if (unit == "kVA") {
        num *= 1000; // Convert kVA to VA
    }

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1) << num;
    std::string result = oss.str();

    // Remove trailing zeros and decimal point if unnecessary
    result.erase(result.find_last_not_of('0') + 1, std::string::npos);
    if (result.back() == '.') {
        result.pop_back();
    }

    return result;
}

int main() {
    std::ifstream infile("test.csv");
    std::ofstream outfile("output.csv");

    if (!infile || !outfile) {
        std::cerr << "Error opening file." << std::endl;
        return 1;
    }

    std::string line;
    std::getline(infile, line);
    outfile << line << std::endl; // Write header

    std::vector<std::string> rows;
    while (std::getline(infile, line)) {
        rows.push_back(line);
    }

    std::tm base_time = {};
    std::istringstream base_ss(rows[0].substr(0, 19));
    base_ss >> std::get_time(&base_time, "%d.%m.%Y %H:%M:%S");
    if (base_ss.fail()) {
        std::cerr << "Failed to parse base timestamp." << std::endl;
        return 1;
    }

    for (const auto& row : rows) {
        std::istringstream ss(row);
        std::string column;
        std::vector<std::string> columns;

        while (std::getline(ss, column, ';')) {
            columns.push_back(column);
        }

        if (columns.size() < 5 || columns[1].empty() || columns[2].empty() || columns[3].empty()) {
            continue; // Skip rows with empty values in power columns
        }

        try {
            int seconds = convertToSeconds(columns[0], base_time);
            outfile << seconds;

            for (size_t i = 1; i < columns.size(); ++i) {
                outfile << ";" << processValue(columns[i]);
            }
            outfile << std::endl;
        } catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
            continue; // Skip row on error
        }
    }

    infile.close();
    outfile.close();

    return 0;
}
