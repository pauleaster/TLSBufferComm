// Simple asynchronous sender program using Boost.Asio library with encryption
// C++ version 14

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

namespace asio = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = asio::ip::tcp;

class Sender
{
private:
    // Member variables and private methods...

    std::string certificateEnvName_;
    std::string privateKeyEnvName_;
    std::string ip_;
    std::string port_;

    std::string getEnvVariable(const std::string &name) const
    {
        const char *value = std::getenv(name.c_str());
        if (!value)
        {
            throw std::runtime_error(name + " environment variable not provided.");
        }
        return value;
    }

    void handleConnect(const boost::system::error_code &error)
    {
        if (!error)
        {
            std::cout << "Connected to the receiver" << std::endl;
        }
        else
        {
            std::cout << "Connect error: " << error.message() << std::endl;
        }
    }

    void handleWrite(const boost::system::error_code &error, size_t bytesTransferred)
    {
        if (!error)
        {
            std::cout << "Message sent" << std::endl;
        }
        else
        {
            std::cout << "Write error: " << error.message() << std::endl;
        }
    }

public:
    Sender &set_certificate(const std::string &certificateEnvName)
    {
        certificateEnvName_ = certificateEnvName;
        return *this;
    }

    Sender &set_private_key(const std::string &privateKeyEnvName)
    {
        privateKeyEnvName_ = privateKeyEnvName;
        return *this;
    }

    Sender &set_IP(const std::string &ip)
    {
        ip_ = ip;
        return *this;
    }

    Sender &set_port(const std::string &port)
    {
        port_ = port;
        return *this;
    }

    void send(const std::string &data)
    {

        asio::io_service ioService_;
        ssl::context ctx_(ssl::context::sslv23);

        ctx_.use_certificate(asio::buffer(getEnvVariable(certificateEnvName_)), ssl::context::pem);
        ctx_.use_private_key(asio::buffer(getEnvVariable(privateKeyEnvName_)), ssl::context::pem);

        ssl::stream<tcp::socket> socket_(ioService_, ctx_);

        tcp::resolver resolver(ioService_);
        tcp::resolver::query query(ip_, port_);
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

        asio::async_connect(socket_.lowest_layer(), endpoint_iterator,
                            [this, data, &socket_](const boost::system::error_code &error, const tcp::resolver::iterator &endpoint_iterator)
                            {
                                handleConnect(error);
                                if (!error)
                                {
                                    socket_.async_handshake(ssl::stream_base::client,
                                                            [this, data, &socket_](const boost::system::error_code &error)
                                                            {
                                                                if (!error)
                                                                {
                                                                    asio::async_write(socket_, asio::buffer(data),
                                                                                      [this, &socket_](const boost::system::error_code &error, size_t bytesTransferred)
                                                                                      {
                                                                                          handleWrite(error, bytesTransferred);
                                                                                          socket_.shutdown();
                                                                                      });
                                                                }
                                                                else
                                                                {
                                                                    std::cout << "Handshake error: " << error.message() << std::endl;
                                                                }
                                                            });
                                }
                            });

        ioService_.run();
    }
};

int main()
{
    try
    {
        Sender sender;
        sender.set_certificate("EB_SENDER_CERTIFICATE_DATA")
            .set_private_key("EB_SENDER_PRIVATE_KEY_DATA")
            .set_IP("127.0.0.1")
            .set_port("1234")
            .send("Hello from sender");
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
