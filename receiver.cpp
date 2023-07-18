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

std::string Receiver::run()
{
    startListening();
    acceptConnection();
    initialiseSSL();
    doHandshake();
    msg = receiveData();
    closeSocket();
    return msg;
}

void Receiver::startListening()
{
    endpoint = tcp::endpoint(asio::ip::address::from_string(receiverIP), port);

    acceptor.open(endpoint.protocol(), error);
    if (error)
    {
        std::cout << "Failed to open acceptor: " << error.message() << std::endl;
        return;
    }

    // Set the SO_REUSEADDR option before binding the acceptor
    acceptor.set_option(asio::socket_base::reuse_address(true));
    
    acceptor.bind(endpoint, error);
    if (error)
    {
        std::cout << "Failed to bind acceptor: " << error.message() << std::endl;
        return;
    }
    acceptor.listen(asio::socket_base::max_listen_connections, error);
    if (error)
    {
        std::cout << "Failed to listen on acceptor: " << error.message() << std::endl;
        return;
    }
    std::cout << "Listening on acceptor." << std::endl;
}

void Receiver::acceptConnection()
{
    acceptor.accept(*socket, error);
    if (error)
    {
        std::cout << "Failed to accept connection: " << error.message() << std::endl;
        return;
    }
}

void Receiver::doHandshake()
{

    // Check SSL socket before handshaking
    if (sslSocket)
    {
        sslSocket->handshake(ssl::stream_base::server, error);
        if (error)
        {
            std::cout << "Handshake failed: " << error.message() << std::endl;
            return;
        }
        std::cout << "Handshake completed." << std::endl;
    }
    else
    {
        std::cout << "SSL socket not initialized." << std::endl;
        return;
    }
}

std::string Receiver::receiveData()
{
    // Receive data
    std::cout << "Receiving data..." << std::endl;
    bytesRead = sslSocket->read_some(boost::asio::buffer(buffer), error);
    io_context.run();

    if (error == boost::asio::error::eof)
    {
        std::cout << "Connection closed by peer" << std::endl;
        return("");
    }
    else if (error)
    {
        std::cout << "Error: " << error.message() << std::endl;
        return("");
    }
    else
    {
        return (std::string(buffer.data(), bytesRead));
    }
    
}

void Receiver::closeSocket()
{
    // Check SSL socket before closing
    std::cout << "Closing socket..." << std::endl;
    if (sslSocket)
    {

        // Close the socket
        std::cout << "Shutting down SSL socket..." << std::endl;
        sslSocket->shutdown(error);
        if (error)
        {
            std::cout << "Failed to shutdown SSL socket: " << error.message() << std::endl;
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
