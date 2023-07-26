// server.cpp
#include "server.hpp"

Server::Server(const std::string &certificateEnvVar, const std::string &privateKeyEnvVar, const std::string serverIP, const unsigned short port)
    : io_context(),
      ctx(ssl::context::tlsv13),
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
    serverIP_ = serverIP;
    port_ = port;
    certificateData = getEnvVariable(certificateEnvVar);
    privateKeyData = getEnvVariable(privateKeyEnvVar);

    if (certificateData.empty() || privateKeyData.empty())
    {
        std::cerr << "Server certificate data or private key data not provided." << std::endl;
        return;
    }
    else
    {
        printf("length of serverCertificateData = %lu\n", certificateData.size());
        printf("length of serverPrivateKeyData = %lu\n", privateKeyData.size());
    }

    ctx.use_certificate(asio::buffer(certificateData, certificateData.size()), ssl::context::pem);
    ctx.use_private_key(asio::buffer(privateKeyData, privateKeyData.size()), ssl::context::pem);
}

std::string Server::getEnvVariable(const std::string &varName)
{
    char *value = std::getenv(varName.c_str());

    // // Debug output
    // std::cout << "Environment variable name: \n" << varName << "\n" << std::endl;
    // std::cout << "Environment variable value: \n" << (value ? value : "(null)") << "\n" << std::endl;

    return value ? value : "";
}

void Server::startListening()
{
    endpoint = tcp::endpoint(asio::ip::address::from_string(serverIP_), port_);

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
    // output listening message with IP address and port
    std::cout << "Listening on " << serverIP_ << ":" << port_ << std::endl;
}

void Server::acceptConnection()
{
    acceptor.accept(*socket, error);
    if (error)
    {
        std::cout << "Failed to accept connection: " << error.message() << std::endl;
        return;
    }
}

void Server::initialiseSSL()
{
    sslSocket.emplace(std::move(*socket), ctx);
}

void Server::doHandshake()
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

std::string Server::receiveData()
{
    // Receive data
    std::cout << "Receiving data..." << std::endl;
    bytesRead = sslSocket->read_some(boost::asio::buffer(buffer), error);
    io_context.run();

    if (error == boost::asio::error::eof)
    {
        std::cout << "Connection closed by peer" << std::endl;
        return ("");
    }
    else if (error)
    {
        std::cout << "Error: " << error.message() << std::endl;
        return ("");
    }
    else
    {
        return (std::string(buffer.data(), bytesRead));
    }
}

void Server::closeSocket()
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

std::string Server::run()
{
    startListening();
    acceptConnection();
    initialiseSSL();
    doHandshake();
    msg = receiveData();
    closeSocket();
    return msg;
}
