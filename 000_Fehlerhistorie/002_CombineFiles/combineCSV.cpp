// Copied from https://www.w3resource.com/cpp-exercises/file-handling/cpp-file-handling-exercise-6.php as starting reference
// Removing ',' and replacing it with ';' as delimiter for csv file, only the first line

#include <iomanip>
#include <iostream>   // Including the input/output stream library
#include <fstream>    // Including the file stream library
#include <string>     // Including the std::string handling library


// Function to display the content of a file
// void displayFileContent(const std::string & filename) {
//     std::ifstream file(filename); // Open file with given filename
//     std::string line; // Declare a std::string to store each line of text

//     if (file.is_open()) { // Check if the file was successfully opened
//         std::cout << "File content:" << std::endl; // Displaying a message indicating file content
//         while (getline(file, line)) { // Read each line from the file
//         std::cout << line << std::endl; // Display each line of the file
//         }
//         file.close(); // Close the file
//     } else {
//         std::cout << "Failed to open the file." << std::endl; // Display an error message if file opening failed
//     }
// }

int main() {

    bool firstLineExecuted = false;
    bool xTimeStampRecorded = false;
    unsigned long long test = 0;
    unsigned long long timeStamp_a = 0;
    unsigned long long timeStamp_b = 0;
    unsigned short count = 1;
    const unsigned short cSecondLine = 2;
    std::string timeStamp;
    
    time_t start, end;

    time(&start);
    std::ifstream inputFile("test_a.csv"); // Open the input file named "test.txt" for reading
    std::ifstream inputFileSecond("test_b.csv"); // Open second input file named "test.txt" for reading
    std::ofstream outputFile("new_test.csv"); // Create or overwrite the output file named "new_test.txt" for writing

    if (inputFile.is_open() && inputFileSecond.is_open() && outputFile.is_open()) { // Check if both input and output files were successfully opened
        std::string line; // Declare a std::string variable to store each line of text
        std::string searchWord = ";"; // Define the word to search for
        std::string replaceWord = ","; // Define the word to replace with

        //displayFileContent("test.csv"); // Display the content of the input file before find and replace

        while (std::getline(inputFile, line)) { // Loop through each line in the input file

            //std::cout << "executed"<< std::endl;
            //std::cout << line << std::endl;
 
            if (count == cSecondLine){
                size_t pos = line.find(searchWord); // Find the position of the search word in the line
                while (pos != std::string::npos && xTimeStampRecorded == false) {
                    timeStamp = line.substr(0, pos);
                    std::cout << timeStamp<< std::endl;
                    timeStamp_a = std::stoll(timeStamp);
                    std::cout << timeStamp_a<< std::endl;
                    xTimeStampRecorded = true;
                    //std::cout << "executed A"<< std::endl;    
                }
            count = 1;
            xTimeStampRecorded = false;
            break;
            }
            count = count + 1;
            //outputFile << line << "\n"; // Write the modified line to the output file
  
        }

        while (std::getline(inputFileSecond, line)) { // Loop through each line in the input file

            //std::cout << "executed"<< std::endl;
            //std::cout << line << std::endl;
 
            if (count == cSecondLine){
                size_t pos = line.find(searchWord); // Find the position of the search word in the line
                while (pos != std::string::npos && xTimeStampRecorded == false) {
                    timeStamp = line.substr(0, pos);
                    std::cout << timeStamp<< std::endl;
                    timeStamp_b = std::stoll(timeStamp);
                    std::cout << timeStamp_b<< std::endl;
                    xTimeStampRecorded = true;
                    //std::cout << "executed A"<< std::endl;    
                }
            count = 1;
            xTimeStampRecorded = false;
            break;
            }
            count = count + 1;
            //outputFile << line << "\n"; // Write the modified line to the output file
  
        }

        if (timeStamp_a < timeStamp_b){
            while (std::getline(inputFile, line)) {
                size_t pos = line.find(timeStamp); // Find the position of the search word in the line
                while (pos != std::string::npos) {
                    break;
                }
            outputFile << line << "\n"; // Write the modified line to the output file
            std::cout << pos << std::endl;    
            }
        }

        inputFile.close(); // Close the input file
        inputFileSecond.close(); // Close the 2nd file
        outputFile.close(); // Close the output file

        std::cout << "\nExecuted successfully." << std::endl; // Display a success message
    } 
    else {
        std::cout << "\nFailed to open the files." << std::endl; // Display an error message if file opening failed
    }
   

    time(&end);

    // Calculating total time taken by the program. 
    double time_taken = double(end - start); 

    std::cout << "Time taken by program is : " << std::fixed 
        << time_taken << std::setprecision(5); 
    std::cout << " sec " << std::endl; 

    return 0; // Return 0 to indicate successful execution
}
