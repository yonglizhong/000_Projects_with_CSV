#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <chrono>
#include <thread>
#include <mutex>
#include <cstdlib>
#include <sstream>
#include <algorithm>

#ifdef _WIN32
    #include <conio.h>
    int getchWrapper() { return _getch(); }
#else
    #include <termios.h>
    #include <unistd.h>
    int getchWrapper() {
        struct termios oldt, newt;
        int ch;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        ch = getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return ch;
    }
#endif

struct PathConfig {
    std::string inputPath;
    std::string outputPath;
};

const std::vector<PathConfig> pathPresets = {
    {"P:/Fehler Historie & Wartungsplan der Anlagen/1_AG1A/RawData/", "P:/Fehler Historie & Wartungsplan der Anlagen/1_AG1A/2024/"},      // Preset 1
    {"P:/Fehler Historie & Wartungsplan der Anlagen/2_SL1B/RawData/", "P:/Fehler Historie & Wartungsplan der Anlagen/2_SL1B/2024/"},        // Preset 2
    {"P:/Fehler Historie & Wartungsplan der Anlagen/3_SL1A/RawData/", "P:/Fehler Historie & Wartungsplan der Anlagen/3_SL1A/2024/"},  // Preset 3
    {"P:/Fehler Historie & Wartungsplan der Anlagen/4_SEED1A/RawData/", "P:/Fehler Historie & Wartungsplan der Anlagen/4_SEED1A/2024/"},        // Preset 4
    {"P:/Fehler Historie & Wartungsplan der Anlagen/5_DECK1A/RawData/", "P:/Fehler Historie & Wartungsplan der Anlagen/5_DECK1A/2024/"},    // Preset 5
    {"P:/Fehler Historie & Wartungsplan der Anlagen/6_ISD1A/RawData/",  "P:/Fehler Historie & Wartungsplan der Anlagen/6_ISD1A/2024/"}     // Preset 6
};

struct ThreadResult {
    std::vector<std::string> outputLines;
    std::unordered_map<std::string, unsigned long long> criteriaCounts;
    unsigned long long removedCount = 0;
};

std::string normalizeLine(const std::string& line) {
    std::stringstream ss(line);
    std::string segment;
    std::vector<std::string> parts;
    
    while (std::getline(ss, segment, ';')) {
        if (segment.size() >= 2 && segment.front() == '"' && segment.back() == '"') {
            segment = segment.substr(1, segment.size() - 2);
        }
        segment.erase(0, segment.find_first_not_of(" \t"));
        segment.erase(segment.find_last_not_of(" \t") + 1);
        if (!segment.empty()) parts.push_back(segment);
    }
    
    std::string result;
    for (size_t i = 0; i < parts.size(); ++i) {
        result += parts[i];
        if (i != parts.size() - 1) result += ";";
    }
    return result;
}

double extractTimeMs(const std::string& line) {
    size_t pos = line.find(';');
    if (pos == std::string::npos) return 0.0;
    try {
        return std::stod(line.substr(0, pos));
    } catch (...) {
        return 0.0;
    }
}

void processBlock(const std::vector<std::string>& lines,
                  size_t startIndex,
                  size_t endIndex,
                  const std::vector<std::string>& searchWords,
                  ThreadResult& result)
{
    for (const auto &word : searchWords) {
        result.criteriaCounts[word] = 0;
    }

    for (size_t i = startIndex; i < endIndex; ++i) {
        const std::string& line = lines[i];
        bool lineRemoved = false;
        for (const auto& word : searchWords) {
            if (line.find(word) != std::string::npos) {
                result.criteriaCounts[word]++;
                lineRemoved = true;
            }
        }
        if (lineRemoved) {
            result.removedCount++;
        } else {
            result.outputLines.push_back(line);
        }
    }
}

int main() {
    int pathChoice;
    std::string inputFilename, outputFilename;
    
    std::cout << "Select path configuration (1-6):\n";
    for(int i = 0; i < pathPresets.size(); ++i) {
        std::cout << i+1 << ". Input: " << pathPresets[i].inputPath 
                  << "\n   Output: " << pathPresets[i].outputPath << "\n";
    }
    std::cout << "Your choice: ";
    std::cin >> pathChoice;
    std::cin.ignore();

    if(pathChoice < 1 || pathChoice > 6) {
        std::cerr << "Invalid path selection!" << std::endl;
        return 1;
    }

    std::cout << "Enter input filename (without path): ";
    std::getline(std::cin, inputFilename);
    std::cout << "Enter output filename (without path): ";
    std::getline(std::cin, outputFilename);

    const PathConfig& config = pathPresets[pathChoice-1];
    std::string fullInputPath = config.inputPath + inputFilename;
    std::string fullOutputPath = config.outputPath + outputFilename;

    std::ifstream inputFile(fullInputPath);
    if (!inputFile.is_open()) {
        std::cerr << "Failed to open input file: " << fullInputPath << std::endl;
        return 1;
    }

    std::string headerLine;
    std::vector<std::string> lines;
    if (std::getline(inputFile, headerLine)) {
        std::string line;
        while (std::getline(inputFile, line)) {
            if (!line.empty()) lines.push_back(line);
        }
    }
    inputFile.close();

    std::vector<std::string> searchWords = {
        "License Key",         // Example criteria
        "berlast: Skript",    // Add more criteria as needed
        "berlast: Zu",
        "Adressfehler Steuerung",
        "Es wurden", // Example additional search phrase  
        "Fehlerzustand beendet",  
        "Datensatzbearbeitung nicht",
        "Warnung Thermoelement Bandheizer Zone",
        "Fehler Bandheizer Heizkreis", 
        "Error SQL Time"
    };

    unsigned int numThreads = std::thread::hardware_concurrency();
    if (numThreads == 0) numThreads = 2;
    if (lines.size() < numThreads) numThreads = lines.size();

    std::vector<ThreadResult> threadResults(numThreads);
    std::vector<std::thread> threads;
    size_t totalLines = lines.size();
    size_t blockSize = totalLines / numThreads;
    size_t remainder = totalLines % numThreads;

    auto startTime = std::chrono::steady_clock::now();

    size_t startIndex = 0;
    for (unsigned int i = 0; i < numThreads; ++i) {
        size_t currentBlockSize = blockSize + (i < remainder ? 1 : 0);
        size_t endIndex = startIndex + currentBlockSize;
        threads.emplace_back(processBlock, std::cref(lines), startIndex, endIndex,
                            std::cref(searchWords), std::ref(threadResults[i]));
        startIndex = endIndex;
    }

    for (auto& t : threads) t.join();

    std::vector<std::string> finalOutput;
    unsigned long long totalRemovedCount = 0;
    std::unordered_map<std::string, unsigned long long> finalCriteriaCounts;
    
    for (const auto &result : threadResults) {
        totalRemovedCount += result.removedCount;
        finalOutput.insert(finalOutput.end(), result.outputLines.begin(), result.outputLines.end());
        for (const auto &entry : result.criteriaCounts) {
            finalCriteriaCounts[entry.first] += entry.second;
        }
    }

    std::sort(finalOutput.begin(), finalOutput.end(), [](const std::string& a, const std::string& b) {
        return extractTimeMs(a) < extractTimeMs(b);
    });

    std::unordered_set<std::string> seenLines;
    std::vector<std::string> dedupedOutput;
    for (const auto& line : finalOutput) {
        std::string normalized = normalizeLine(line);
        if (seenLines.insert(normalized).second) {
            dedupedOutput.push_back(line);
        }
    }
    finalOutput = std::move(dedupedOutput);

    std::ofstream outputFile(fullOutputPath);
    if (!outputFile.is_open()) {
        std::cerr << "Failed to open output file: " << fullOutputPath << std::endl;
        return 1;
    }
    outputFile << headerLine << "\n";
    for (const auto& outLine : finalOutput) {
        outputFile << outLine << "\n";
    }
    outputFile.close();

    auto endTime = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsedSeconds = endTime - startTime;

    std::cout << "\nProcessing complete!\n";
    std::cout << "Total lines processed: " << lines.size() << "\n";
    std::cout << "Lines removed: " << totalRemovedCount << "\n";
    std::cout << "Unique lines saved: " << finalOutput.size() << "\n";
    std::cout << "Execution time: " << elapsedSeconds.count() << " seconds\n";
    std::cout << "Removal breakdown:\n";
    for (const auto &entry : finalCriteriaCounts) {
        std::cout << "  " << entry.first << ": " << entry.second << "\n";
    }

    std::cout << "\nPress ESC to exit...\n";
    while (true) {
        if (getchWrapper() == 27) break;
    }

    return 0;
}
 