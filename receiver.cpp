// receiver.cpp
#include "receiver.hpp"

const std::string receiverIP = "127.0.0.1"; // Use the loopback address (localhost)
const unsigned short port = 4321;

Receiver::Receiver(const std::string &certificateEnvVar, const std::string &privateKeyEnvVar)
    : io_context(),
      ctx(ssl::context::sslv23),
      endpoint(),
      acceptor(io_context),
      socket(std::make_shared<tcp::socket>(io_context)),
      buffer(),
      error(),
      bytesRead(),
      certificateData(),
      privateKeyData(),
      sslSocket() // Initialize sslSocket as empty (std::nullopt)
{
    certificateData = getEnvVariable(certificateEnvVar);
    privateKeyData = getEnvVariable(privateKeyEnvVar);

    if (certificateData.empty() || privateKeyData.empty())
    {
        std::cerr << "Receiver certificate data or private key data not provided." << std::endl;
        return;
    }
    else
    {
        printf("length of receiverCertificateData = %lu\n", certificateData.size());
        printf("length of receiverPrivateKeyData = %lu\n", privateKeyData.size());
    }

    ctx.use_certificate(asio::buffer(certificateData, certificateData.size()), ssl::context::pem);
    ctx.use_private_key(asio::buffer(privateKeyData, privateKeyData.size()), ssl::context::pem);
}

void Receiver::initialiseSSL()
{
    sslSocket.emplace(std::move(*socket), ctx);
}

void Receiver::run()
{

    endpoint = tcp::endpoint(asio::ip::address::from_string(receiverIP), port);

    boost::system::error_code ec;
    acceptor.open(endpoint.protocol(), ec);
    if (ec)
    {
        std::cout << "Failed to open acceptor: " << ec.message() << std::endl;
        return;
    }
    acceptor.bind(endpoint, ec);
    if (ec)
    {
        std::cout << "Failed to bind acceptor: " << ec.message() << std::endl;
        return;
    }
    acceptor.listen(asio::socket_base::max_listen_connections, ec);
    if (ec)
    {
        std::cout << "Failed to listen on acceptor: " << ec.message() << std::endl;
        return;
    }
    std::cout << "Listening on acceptor." << std::endl;

    acceptor.accept(*socket, ec);
    if (ec)
    {
        std::cout << "Failed to accept connection: " << ec.message() << std::endl;
        return;
    }

    // Initialize the SSL socket after acceptor.accept()
    initialiseSSL();

    // Check SSL socket before handshaking
    if (sslSocket)
    {
        sslSocket->handshake(ssl::stream_base::server, ec);
        if (ec)
        {
            std::cout << "Handshake failed: " << ec.message() << std::endl;
            return;
        }
        std::cout << "Handshake completed." << std::endl;

        // Receive data
        bytesRead = sslSocket->read_some(boost::asio::buffer(buffer), error);
        io_context.run();

        if (error == boost::asio::error::eof)
        {
            std::cout << "Connection closed by peer" << std::endl;
        }
        else if (error)
        {
            std::cout << "Error: " << error.message() << std::endl;
        }
        else
        {
            std::cout << "Received " << bytesRead << " bytes" << std::endl;
            std::cout << "Message: " << std::string(buffer.data(), bytesRead) << std::endl;
        }

        // Close the socket
        sslSocket->shutdown(ec);
        if (ec)
        {
            std::cout << "Failed to shutdown SSL socket: " << ec.message() << std::endl;
            return;
        }
    }
    else
    {
        std::cout << "Failed to initialise SSL socket" << std::endl;
    }
}

std::string Receiver::getEnvVariable(const std::string &varName)
{
    char *value = std::getenv(varName.c_str());

    // // Debug output
    // std::cout << "Environment variable name: \n" << varName << "\n" << std::endl;
    // std::cout << "Environment variable value: \n" << (value ? value : "(null)") << "\n" << std::endl;

    return value ? value : "";
}
