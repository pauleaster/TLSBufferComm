// Simple sender program using boost asio library
// C++ version 14

#include <iostream>
#include <string>
#include <boost/asio.hpp>

int main() {
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::socket socket(io_service);

    // Connect to the receiver
    boost::asio::ip::tcp::endpoint endpoint(
        boost::asio::ip::address::from_string("127.0.0.1"), 1234);
    socket.connect(endpoint);

    // Send the message
    std::string message = "Hello from sender";
    boost::asio::write(socket, boost::asio::buffer(message));

    std::cout << "Message sent" << std::endl;
    
    // Close the socket
    socket.close();

    return 0;
}