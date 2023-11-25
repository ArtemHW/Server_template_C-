#include <stdio.h>
#include <winsock2.h>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <fstream>

void sendResponse(SOCKET clientSocket, const std::string& response);
void HandleClient(SOCKET clientSocket);
void writeStringToFile(const std::string& filename, const std::string& data);
std::string readStringFromFile(const std::string& filename);

std::vector<std::thread> threads;

int main() {
    

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "WSAStartup failed";
        return 1;
    }
    std::cout << "WSAStartup OK" << std::endl;

    // Create a socket
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        std::cout << "Socket creation failed";
        return 1;
    }
    std::cout << "Socket creation OK" << std::endl;

    // Define the server address
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Bind the socket to the server address
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cout << "Binding failed";
        return 1;
    }
    std::cout << "Binding OK" << std::endl;

    // Listen for incoming connections
    if (listen(serverSocket, 10) == SOCKET_ERROR) {
        std::cout << "Listen failed";
        return 1;
    }
    std::cout << "Listen OK" << std::endl;

    int bytes = 0;
    while(true){
        //accept client request
        struct sockaddr_in clientAddr;
        int clientAddrLen = sizeof(clientAddr);
        SOCKET clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocket == INVALID_SOCKET) {
        std::cout << "Accepting connection failed" << std::endl;
        return 1;
        }
        std::cout << "Accepting connection OK" << std::endl;

        // Start a new thread to handle the client
        threads.emplace_back(HandleClient, clientSocket);

    }

    for (auto& thread : threads) {
        thread.join();
    }

    // Close sockets when done
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}

void HandleClient(SOCKET clientSocket) {
    char buff[30720] = { 0 };
    int bytes = recv(clientSocket, buff, sizeof(buff), 0);

    if (bytes < 0) {
        std::cout << "Couldn't read client request";
    } else {
        std::cout << "Received data: " << std::endl;
        for (int i = 0; i < bytes; ++i) {
            std::cout << buff[i];
        }
        std::cout << std::endl;
        // Parse the request and send a response as before.
        // ...
            // Check if it's a GET request
        if (strstr(buff, "GET") != NULL) {
            // Handle GET request
            std::string response = readStringFromFile("example.txt");
            // Calculate the content length based on the response
            std::string contentLengthHeader = "Content-Length: " + std::to_string(response.length()) + "\n";
            // Construct the complete response with dynamic Content-Length
            std::string httpResponse = "HTTP/1.1 200 OK\nContent-Type: text/html\n" + contentLengthHeader + "\n" + response;

            sendResponse(clientSocket, httpResponse);
        } else if (strstr(buff, "POST") != NULL) {
            // Handle POST request
            std::string response = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 22\n\n";
            response += "<html><h1>POST Request</h1></html>";
            sendResponse(clientSocket, response);
        } else {
            // Handle other request methods
            std::string response = "HTTP/1.1 501 Not Implemented\nContent-Type: text/html\nContent-Length: 28\n\n";
            response += "<html><h1>Not Implemented</h1></html>";
            sendResponse(clientSocket, response);
        }
    }
    std::cout << "Number of threads in vector threads " << threads.size() << std::endl;
    closesocket(clientSocket);
}

void sendResponse(SOCKET clientSocket, const std::string& response) {
    int bytesSent = 0;
    int totalBytesSent = 0;
    while (totalBytesSent < response.size()) {
        bytesSent = send(clientSocket, response.c_str() + totalBytesSent, response.size() - totalBytesSent, 0);
        if (bytesSent < 0) {
            std::cout << "Could not send response";
            break;
        }
        totalBytesSent += bytesSent;
    }
    std::cout << "Sent response to client" << std::endl;
}

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