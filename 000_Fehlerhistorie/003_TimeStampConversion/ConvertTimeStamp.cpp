#include <iostream>   // Including the input/output stream library
#include <fstream>    // Including the file stream library
#include <string>     // Including the string handling library
#include <time.h>
#include<cmath>


int main() {

    long double offSet = 2556750000;
    long double offSet_a = 136799;
    long long count = 0;
    long long lineNumber;

    const long double dayToSecondsFactor = 0.864;
    time_t start, end;
    bool firstLineExecuted = false;
    bool xTimeStampRecorded = false;
    long double timeStamp_a = 0;

    std::string timeStamp;
    float timeStamp_day;
    float timeStamp_ms;
    time_t t;

    std::ifstream inputFile("test.csv"); // Open the input file named "test.txt" for reading
    std::ofstream outputFile("new_test.csv"); // Create or overwrite the output file named "new_test.txt" for writing

    if (inputFile.is_open() && outputFile.is_open()) { // Check if both input and output files were successfully opened
        std::cout << "Please enter a Line Number: ";
        std::cin >> lineNumber;

        std::string line; // Declare a string variable to store each line of text
        std::string searchWord = "."; // Define the word to search for

        while (std::getline(inputFile, line)) { // Loop through each line in the input file

            if (firstLineExecuted == false){    
                firstLineExecuted = true;
            }
            if (count == lineNumber){

                size_t pos = line.find(searchWord); // Find the position of the search word in the line
                while (pos != std::string::npos && xTimeStampRecorded == false) {
                    timeStamp = line.substr(0, pos-1);
                    std::cout << timeStamp << std::endl;
                    timeStamp_a = std::stold(timeStamp);
                    std::cout << timeStamp_a << std::endl;
                    timeStamp_a = timeStamp_a - offSet;
                    std::cout << timeStamp_a << std::endl;
                    // timeStamp_a = timeStamp_a*shift5decimals;
                    // std::cout << timeStamp_a << std::endl;
                    timeStamp_a = timeStamp_a*dayToSecondsFactor;
                    std::cout << timeStamp_a << std::endl;
                    timeStamp_a = timeStamp_a - offSet_a;
                    std::cout << timeStamp_a << std::endl;

                    t = timeStamp_a;          
                    printf("Time: %s",ctime(&t));  
                    //std::cout << ctime(&t) << std::endl;       //time
                    //timeStamp_ms = timeStamp_a*dayToMilliFactor;
                    xTimeStampRecorded = true;
                }
            }
            else {
                count++;
            }

        outputFile << line << "\n"; // Write the line to the output file    
            
        }

        inputFile.close(); // Close the input file
        outputFile.close(); // Close the output file

    } 
    else {
        std::cout << "\nFailed to open the files." << std::endl; // Display an error message if file opening failed
    }

    return 0; // Return 0 to indicate successful execution
}
