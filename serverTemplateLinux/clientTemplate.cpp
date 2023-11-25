#include <iostream>
#include <boost/asio.hpp>

using namespace boost::asio;

int main() {
    try {
        boost::asio::io_service io_service;
        ip::tcp::socket socket(io_service);
        ip::tcp::endpoint endpoint(ip::address::from_string("147.232.194.177"), 12128);

        // Connect to the server
        socket.connect(endpoint);

        // Send a message to the server
        std::string message = "Hello, Server!";
        boost::asio::write(socket, boost::asio::buffer(message + "\n"));

        std::cout << "Message sent to the server." << std::endl;

        // Receive and print the response from the server
        boost::asio::streambuf response_buffer;
        boost::asio::read_until(socket, response_buffer, '\n');
        std::istream response_stream(&response_buffer);
        std::string server_response;
        std::getline(response_stream, server_response);

        std::cout << "Received response from server: " << server_response << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
