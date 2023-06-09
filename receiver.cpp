// Simple receiver program using boost asio library
// C++ version 14

#include <iostream>
#include <array>
#include <boost/asio.hpp>

int main() {
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::acceptor acceptor(
        io_service, boost::asio::ip::tcp::endpoint(
            boost::asio::ip::tcp::v4(), 1234));
    boost::asio::ip::tcp::socket socket(io_service);
    acceptor.accept(socket);

    // Receive data
    std::array<char, 128> buffer;
    boost::system::error_code error;
    size_t bytesRead = socket.read_some(boost::asio::buffer(buffer), error);
    io_service.run();

    if (error == boost::asio::error::eof) {
        std::cout << "Connection closed by peer" << std::endl;
    } else if (error) {
        std::cout << "Error: " << error.message() << std::endl;
    } else {
        std::cout << "Received " << bytesRead << " bytes" << std::endl;
        std::cout << "Message: " << std::string(buffer.data(), bytesRead) << std::endl;
    }
    
    // Close the socket
    socket.close();

    return 0;
}