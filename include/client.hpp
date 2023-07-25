#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

namespace asio = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = asio::ip::tcp;

class Client
{
public:
    Client(const std::string &certificateEnvVar, const std::string &privateKeyEnvVar);
    void connect(const std::string &serverIP, unsigned short port);
    void send(const std::string &message);
    void disconnect();

private:
    void handleConnect(const boost::system::error_code &error);
    void handleWrite(const boost::system::error_code &error, size_t bytesTransferred);
    void checkCertificate();
    void checkPrivateKey();

    asio::io_context io_context_;
    ssl::context ctx_;
    ssl::stream<tcp::socket> socket_;
    bool connected_;
    boost::system::error_code certificateError;
    boost::system::error_code privateKeyError;
    boost::system::error_code handshakeError;
};

#endif // CLIENT_HPP
