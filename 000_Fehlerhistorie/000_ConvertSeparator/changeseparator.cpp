// Copied from https://www.w3resource.com/cpp-exercises/file-handling/cpp-file-handling-exercise-6.php as starting reference
// Removing ',' and replacing it with ';' as delimiter for csv file, only the first line

using namespace std;
#include <iomanip>
#include <iostream>   // Including the input/output stream library
#include <fstream>    // Including the file stream library
#include <string>     // Including the string handling library

// Function to display the content of a file
void displayFileContent(const string & filename) {
    ifstream file(filename); // Open file with given filename
    string line; // Declare a string to store each line of text

    if (file.is_open()) { // Check if the file was successfully opened
        cout << "File content:" << endl; // Displaying a message indicating file content
        while (getline(file, line)) { // Read each line from the file
        cout << line << endl; // Display each line of the file
        }
        file.close(); // Close the file
    } else {
        cout << "Failed to open the file." << endl; // Display an error message if file opening failed
    }
}

int main() {

    bool firstLineExecuted = false;
    time_t start, end;

    time(&start);
    ifstream inputFile("test.csv"); // Open the input file named "test.txt" for reading
    ofstream outputFile("new_test.csv"); // Create or overwrite the output file named "new_test.txt" for writing

    if (inputFile.is_open() && outputFile.is_open()) { // Check if both input and output files were successfully opened
        string line; // Declare a string variable to store each line of text
        string searchWord = ","; // Define the word to search for
        string replaceWord = ";"; // Define the word to replace with

        cout << "Search word:  " << searchWord << endl; // Display the word to search for
        cout << "Replace word:  " << replaceWord << endl; // Display the word to replace with

        //cout << "\nBefore find and replace:" << endl; // Display a message before find and replace
        //displayFileContent("test.csv"); // Display the content of the input file before find and replace

        while (getline(inputFile, line)) { // Loop through each line in the input file

            if (firstLineExecuted == false){
                
                size_t pos = line.find(searchWord); // Find the position of the search word in the line

                while (pos != string::npos) { // Repeat until all occurrences are replaced
                    line.replace(pos, searchWord.length(), replaceWord); // Replace the search word with the replace word
                    pos = line.find(searchWord, pos + replaceWord.length()); // Find the next occurrence of the search word
                    //cout << "\nPos:" << pos << endl;
                }

                firstLineExecuted = true;
            }
            
            outputFile << line << "\n"; // Write the modified line to the output file
        }

        inputFile.close(); // Close the input file
        outputFile.close(); // Close the output file

        //cout << "After find and replace:" << endl; // Display a message after find and replace
        //displayFileContent("new_test.csv"); // Display the content of the output file after find and replace

        cout << "\nWord replaced successfully." << endl; // Display a success message
    } 
    else {
        cout << "\nFailed to open the files." << endl; // Display an error message if file opening failed
    }
   

    time(&end);

    // Calculating total time taken by the program. 
    double time_taken = double(end - start); 

    cout << "Time taken by program is : " << fixed 
        << time_taken << setprecision(5); 
    cout << " sec " << endl; 

    return 0; // Return 0 to indicate successful execution
}
