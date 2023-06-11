#include <iostream>
#include <array>
#include <string>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

namespace asio = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = asio::ip::tcp;

class Receiver
{
private:
    asio::io_service ioService_;
    ssl::context ctx_;
    ssl::stream<tcp::socket> socket_;
    std::array<char, 128> buffer_;

    std::string certificateEnvName_;
    std::string privateKeyEnvName_;
    std::string ip_;
    std::string port_;

    std::string getEnvVariable(const std::string &name) const;

    void handleAccept(const boost::system::error_code &error);
    void handleRead(const boost::system::error_code &error, size_t bytesRead);

public:
    Receiver()
        : ioService_(), ctx_(ssl::context::sslv23), socket_(ioService_, ctx_), buffer_()
    {
    }

    Receiver &set_certificate(const std::string &certificateEnvName);
    Receiver &set_private_key(const std::string &privateKeyEnvName);
    Receiver &set_IP(const std::string &ip);
    Receiver &set_port(const std::string &port);
    std::string receive();
};

std::string Receiver::getEnvVariable(const std::string &name) const
{
    const char *value = std::getenv(name.c_str());
    if (!value)
    {
        throw std::runtime_error(name + " environment variable not provided.");
    }
    return value;
}

void Receiver::handleAccept(const boost::system::error_code &error)
{
    if (!error)
    {
        std::cout << "Accepted a connection" << std::endl;
        socket_.async_handshake(ssl::stream_base::server,
            [this](const boost::system::error_code &error) {
                if (!error)
                {
                    socket_.async_read_some(asio::buffer(buffer_),
                        [this](const boost::system::error_code &error, size_t bytesRead) {
                            handleRead(error, bytesRead);
                            socket_.shutdown();
                        });
                }
                else
                {
                    std::cout << "Handshake error: " << error.message() << std::endl;
                }
            });
    }
    else
    {
        std::cout << "Accept error: " << error.message() << std::endl;
    }
}

void Receiver::handleRead(const boost::system::error_code &error, size_t bytesRead)
{
    if (!error)
    {
        std::cout << "Received " << bytesRead << " bytes" << std::endl;
        std::cout << "Message: " << std::string(buffer_.data(), bytesRead) << std::endl;
    }
    else
    {
        std::cout << "Read error: " << error.message() << std::endl;
    }
}

Receiver &Receiver::set_certificate(const std::string &certificateEnvName)
{
    certificateEnvName_ = certificateEnvName;
    return *this;
}

Receiver &Receiver::set_private_key(const std::string &privateKeyEnvName)
{
    privateKeyEnvName_ = privateKeyEnvName;
    return *this;
}

Receiver &Receiver::set_IP(const std::string &ip)
{
    ip_ = ip;
    return *this;
}

Receiver &Receiver::set_port(const std::string &port)
{
    port_ = port;
    return *this;
}

std::string Receiver::receive()
{
    tcp::endpoint endpoint(asio::ip::address::from_string(ip_), std::stoi(port_));
    tcp::acceptor acceptor(ioService_, endpoint);
    tcp::socket socket_(ioService_);
    acceptor.accept(socket_);

    socket_.handshake(ssl::stream_base::server);

    boost::system::error_code error;
    size_t bytesRead = socket_.read_some(boost::asio::buffer(buffer_), error);

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
        std::cout << "Message: " << std::string(buffer_.data(), bytesRead) << std::endl;
    }

    socket_.shutdown();

    return std::string(buffer_.data(), bytesRead);
}



int main()
{
    try
    {
        Receiver receiver;
        std::string data = receiver.set_certificate("EB_RECEIVER_CERTIFICATE_DATA")
                                 .set_private_key("EB_RECEIVER_PRIVATE_KEY_DATA")
                                 .set_IP("127.0.0.1")
                                 .set_port("1234")
                                 .receive();
        std::cout << "Received data: " << data << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
