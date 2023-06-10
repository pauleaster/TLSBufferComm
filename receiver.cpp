// Simple receiver program using Boost.Asio library with encryption
// C++ version 14

#include <iostream>
#include <array>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

namespace asio = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = asio::ip::tcp;

const std::string receiverIP = "127.0.0.1";  // Use the loopback address (localhost)
const unsigned short port = 1234;

int main() {
    asio::io_service io_service;
    ssl::context ctx(ssl::context::sslv23);

    // Load the certificate and private key files
    const char* receiverCertificateData = std::getenv("EB_RECEIVER_CERTIFICATE_DATA");
    const char* receiverPrivateKeyData = std::getenv("EB_RECEIVER_PRIVATE_KEY_DATA");

    if (!receiverCertificateData || !receiverPrivateKeyData) {
        std::cerr << "Receiver certificate data or private key data not provided." << std::endl;
        return 1;
    }

    ctx.use_certificate(asio::buffer(receiverCertificateData, std::strlen(receiverCertificateData)), ssl::context::pem);
    ctx.use_private_key(asio::buffer(receiverPrivateKeyData, std::strlen(receiverPrivateKeyData)), ssl::context::pem);


    tcp::endpoint endpoint(asio::ip::address::from_string(receiverIP), port);
    tcp::acceptor acceptor(io_service, endpoint);
    tcp::socket socket(io_service);
    acceptor.accept(socket);

    ssl::stream<tcp::socket> sslSocket(std::move(socket), ctx);
    sslSocket.handshake(ssl::stream_base::server);

    // Receive data
    std::array<char, 128> buffer;
    boost::system::error_code error;
    size_t bytesRead = sslSocket.read_some(boost::asio::buffer(buffer), error);
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
    sslSocket.shutdown();

    return 0;
}