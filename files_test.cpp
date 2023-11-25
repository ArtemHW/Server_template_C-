#include <iostream>
#include <fstream>
#include <string>

// Function to write a string to a file
void writeStringToFile(const std::string& filename, const std::string& data) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << data;
        file.close();
        std::cout << "String written to file: " << filename << std::endl;
    } else {
        std::cerr << "Error: Unable to open file for writing." << std::endl;
    }
}

// Function to read a string from a file
std::string readStringFromFile(const std::string& filename) {
    std::ifstream file(filename);
    std::string content;
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            content += line;
            content += '\n'; // Add newline to separate lines
        }
        file.close();
        std::cout << "String read from file: " << filename << std::endl;
    } else {
        std::cerr << "Error: Unable to open file for reading." << std::endl;
    }
    return content;
}

int main() {
    std::string textToWrite = "<html><h1>GET Request</h1></html>";
    writeStringToFile("example.txt", textToWrite);

    std::string readText = readStringFromFile("example.txt");
    std::cout << "Read text from file: " << readText << std::endl;

    return 0;
}
