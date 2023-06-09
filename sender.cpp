// Simple sender program using boost asio library
// C++ version 14

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>



namespace asio = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = asio::ip::tcp;

int main() {
    asio::io_service io_service;
    ssl::context ctx(ssl::context::sslv23);

    // Load the certificate and private key files
    ctx.use_certificate_file("certificate.pem", ssl::context::pem);
    ctx.use_private_key_file("private_key.pem", ssl::context::pem);

    ssl::stream<tcp::socket> socket(io_service, ctx);

    // Connect to the receiver
    tcp::resolver resolver(io_service);
    tcp::resolver::query query("127.0.0.1", "1234");
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    asio::connect(socket.lowest_layer(), endpoint_iterator);
    socket.handshake(ssl::stream_base::client);

    // Send the message
    std::string message = "Hello from sender";
    asio::write(socket, asio::buffer(message));

    std::cout << "Message sent" << std::endl;

    // Close the socket
    socket.shutdown();

    return 0;
}