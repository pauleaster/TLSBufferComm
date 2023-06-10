// Simple asynchronous sender program using Boost.Asio library with encryption
// C++ version 14

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

namespace asio = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = asio::ip::tcp;

void handleConnect(const boost::system::error_code& error) {
    if (!error) {
        std::cout << "Connected to the receiver" << std::endl;
    } else {
        std::cout << "Connect error: " << error.message() << std::endl;
    }
}

void handleWrite(const boost::system::error_code& error, size_t bytesTransferred) {
    if (!error) {
        std::cout << "Message sent" << std::endl;
    } else {
        std::cout << "Write error: " << error.message() << std::endl;
    }
}

int main() {
    asio::io_service io_service;
    ssl::context ctx(ssl::context::sslv23);

    // Load the certificate and private key files
    const char* senderCertificateData = std::getenv("EB_SENDER_CERTIFICATE_DATA");
    const char* senderPrivateKeyData = std::getenv("EB_SENDER_PRIVATE_KEY_DATA");

if (!senderCertificateData || !senderPrivateKeyData) {
    std::cerr << "Sender certificate data or private key data not provided." << std::endl;
    return 1;
}

ctx.use_certificate(asio::buffer(senderCertificateData, std::strlen(senderCertificateData)), ssl::context::pem);
ctx.use_private_key(asio::buffer(senderPrivateKeyData, std::strlen(senderPrivateKeyData)), ssl::context::pem);


    ssl::stream<tcp::socket> socket(io_service, ctx);

    // Connect to the receiver
    tcp::resolver resolver(io_service);
    tcp::resolver::query query("127.0.0.1", "1234");
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

    asio::async_connect(
        socket.lowest_layer(),
        endpoint_iterator,
        [&socket](const boost::system::error_code& error, const tcp::resolver::iterator& endpoint_iterator ) {
            handleConnect(error);
            if (!error) {
                socket.async_handshake(
                    ssl::stream_base::client,
                    [&socket](const boost::system::error_code& error) {
                        if (!error) {
                            // Send the message
                            std::string message = "Hello from sender";
                            asio::async_write(
                                socket,
                                asio::buffer(message),
                                [&socket](const boost::system::error_code& error, size_t bytesTransferred) {
                                    handleWrite(error, bytesTransferred);
                                    // Close the socket
                                    socket.shutdown();
                                });
                        } else {
                            std::cout << "Handshake error: " << error.message() << std::endl;
                        }
                    });
            }
        });

    io_service.run();

    return 0;
}
