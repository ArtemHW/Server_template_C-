#include <iostream>
#include <boost/asio.hpp>

using namespace boost::asio;

class AsyncServer {
public:
    AsyncServer(io_service& io_service, const std::string& ip, short port)
        : acceptor_(io_service, ip::tcp::endpoint(ip::address::from_string(ip), port)),
        socket_(io_service) {
        startAccept();
    }

private:
    void startAccept() {
        acceptor_.async_accept(socket_,
            [this](const boost::system::error_code& ec) {
                handleAccept(ec);
            });
    }

    void handleAccept(const boost::system::error_code& ec) {
        if (!ec) {
            std::cout << "Client connected: " << socket_.remote_endpoint() << std::endl;
            startRead(); // Start reading from the connected client
        } else {
            if (ec != boost::asio::error::operation_aborted) {
                std::cerr << "Error accepting connection: " << ec.message() << std::endl;
            }
        }

        startAccept(); // Start accepting the next connection
    }

    void startRead() {
        async_read_until(socket_, buffer_, '\n',
            [this](const boost::system::error_code& ec, std::size_t bytes_transferred) {
                handleRead(ec, bytes_transferred);
            });
    }

    void handleRead(const boost::system::error_code& ec, std::size_t bytes_transferred) {
        if (!ec) {
            // Process the received message
            std::istream is(&buffer_);
            std::string client_message;
            std::getline(is, client_message);

            std::cout << "Received message from client: " << client_message << std::endl;

            // Send a response back to the client
            std::string response_message = "Hello, Client! I received your message.";
            boost::asio::write(socket_, boost::asio::buffer(response_message + "\n"));

            startRead(); // Continue reading from the same client
        } else {
            std::cerr << "Error reading from client: " << ec.message() << std::endl;
            socket_.close();
        }
    }

    ip::tcp::acceptor acceptor_;
    ip::tcp::socket socket_;
    streambuf buffer_;
};

int main() {
    try {
        boost::asio::io_service io_service;
        std::string server_ip = "147.232.194.177"; // Change this to your desired IP address
        AsyncServer server(io_service, server_ip, 12128); // Change the port as needed

        io_service.run();
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}