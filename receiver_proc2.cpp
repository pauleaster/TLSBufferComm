// Simple receiver program using Boost.Asio library with encryption
// C++ version 20

#define BOOST_ASIO_ENABLE_HANDLER_TRACKING 2
#include <iostream>
#include <array>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <optional>

namespace asio = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = asio::ip::tcp;

const std::string receiverIP = "127.0.0.1"; // Use the loopback address (localhost)
const unsigned short port = 4321;

class Receiver_vars
{
public:
    asio::io_context io_context;
    ssl::context ctx;
    std::string receiverCertificateData;
    std::string receiverPrivateKeyData;
    tcp::endpoint endpoint;
    tcp::acceptor acceptor;
    std::shared_ptr<tcp::socket> socket;                 // Using shared_ptr for socket
    std::optional<ssl::stream<tcp::socket>> sslSocket; // Use std::optional for ssl::stream
    std::array<char, 128> buffer;
    boost::system::error_code error;
    size_t bytesRead;

    Receiver_vars()
        : io_context(),
          ctx(ssl::context::sslv23),
          receiverCertificateData(),
          receiverPrivateKeyData(),
          endpoint(),
          acceptor(io_context),
          socket(std::make_shared<tcp::socket>(io_context)),
          buffer(),
          error(),
          bytesRead(),
          sslSocket() // Initialize sslSocket as empty (std::nullopt)
    {
    }

    // Function to initialize sslSocket after accept() is successful
    void initialiseSSL()
    {
        sslSocket.emplace(std::move(*socket), ctx);
    }
};

int main()
{
    Receiver_vars rv;

    // Load the certificate and private key files
    rv.receiverCertificateData = std::getenv("EB_RECEIVER_CERTIFICATE_DATA");
    rv.receiverPrivateKeyData = std::getenv("EB_RECEIVER_PRIVATE_KEY_DATA");

    if (rv.receiverCertificateData.empty() || rv.receiverPrivateKeyData.empty())
    {
        std::cerr << "Receiver certificate data or private key data not provided." << std::endl;
        return 1;
    }
    else
    {
        printf("length of receiverCertificateData = %lu\n", rv.receiverCertificateData.size());
        printf("length of receiverPrivateKeyData = %lu\n", rv.receiverPrivateKeyData.size());
    }

    rv.ctx.use_certificate(asio::buffer(rv.receiverCertificateData, rv.receiverCertificateData.size()), ssl::context::pem);
    rv.ctx.use_private_key(asio::buffer(rv.receiverPrivateKeyData, rv.receiverPrivateKeyData.size()), ssl::context::pem);

    rv.endpoint = tcp::endpoint(asio::ip::address::from_string(receiverIP), port);

    boost::system::error_code ec;
    rv.acceptor.open(rv.endpoint.protocol(), ec);
    if (ec)
    {
        std::cout << "Failed to open acceptor: " << ec.message() << std::endl;
        return 1;
    }
    rv.acceptor.bind(rv.endpoint, ec);
    if (ec)
    {
        std::cout << "Failed to bind acceptor: " << ec.message() << std::endl;
        return 1;
    }
    rv.acceptor.listen(asio::socket_base::max_listen_connections, ec);
    if (ec)
    {
        std::cout << "Failed to listen on acceptor: " << ec.message() << std::endl;
        return 1;
    }
    std::cout << "Listening on acceptor." << std::endl;

    rv.acceptor.accept(*rv.socket, ec);
    if (ec)
    {
        std::cout << "Failed to accept connection: " << ec.message() << std::endl;
        return 1;
    }
    
    // Initialize the SSL socket after acceptor.accept()
    rv.initialiseSSL();

    // Check SSL socket before handshaking
    if (rv.sslSocket)
    {
        rv.sslSocket->handshake(ssl::stream_base::server, ec);
        if (ec)
        {
            std::cout << "Handshake failed: " << ec.message() << std::endl;
            return 1;
        }
        std::cout << "Handshake completed." << std::endl;

        // Receive data
        rv.bytesRead = rv.sslSocket->read_some(boost::asio::buffer(rv.buffer), rv.error);
        rv.io_context.run();

        if (rv.error == boost::asio::error::eof)
        {
            std::cout << "Connection closed by peer" << std::endl;
        }
        else if (rv.error)
        {
            std::cout << "Error: " << rv.error.message() << std::endl;
        }
        else
        {
            std::cout << "Received " << rv.bytesRead << " bytes" << std::endl;
            std::cout << "Message: " << std::string(rv.buffer.data(), rv.bytesRead) << std::endl;
        }

        // Close the socket
        rv.sslSocket->shutdown(ec);
        if (ec)
        {
            std::cout << "Failed to shutdown SSL socket: " << ec.message() << std::endl;
            return 1;
        }
    }
    else
    {
        std::cout << "Failed to initialise SSL socket" << std::endl;
    }

    return 0;
}
