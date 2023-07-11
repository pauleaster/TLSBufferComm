#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

namespace asio = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = asio::ip::tcp;

class Receiver
{
public:
    Receiver(const std::string &certificateEnvVar, const std::string &privateKeyEnvVar)
        : ctx_(ssl::context::sslv23),
          socket_(io_service_, ctx_),
          connected_(false)
    {
        const char *certificateData = std::getenv(certificateEnvVar.c_str());
        const char *privateKeyData = std::getenv(privateKeyEnvVar.c_str());

        if (!certificateData || !privateKeyData)
        {
            std::cerr << "Receiver certificate data or private key data not provided." << std::endl;
            throw std::runtime_error("Certificate or private key data missing.");
        }

        ctx_.use_certificate(asio::buffer(certificateData, std::strlen(certificateData)), ssl::context::pem);
        ctx_.use_private_key(asio::buffer(privateKeyData, std::strlen(privateKeyData)), ssl::context::pem);
    }

    void connect()
    {
        if (connected_)
        {
            std::cerr << "Already connected." << std::endl;
            return;
        }

        tcp::resolver resolver(io_service_);
        tcp::resolver::query query("127.0.0.1", "1234");
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

        asio::async_connect(
            socket_.lowest_layer(),
            endpoint_iterator,
            [this](const boost::system::error_code &error, const tcp::resolver::iterator &endpoint_iterator)
            {
                handleConnect(error);
                if (!error)
                {
                    socket_.async_handshake(
                        ssl::stream_base::server,
                        [this](const boost::system::error_code &error)
                        {
                            if (!error)
                            {
                                connected_ = true;
                                std::cout << "Connected to the sender" << std::endl;
                            }
                            else
                            {
                                std::cout << "Handshake error: " << error.message() << std::endl;
                            }
                        });
                }
            });

        io_service_.run();
    }

    std::string receive()
    {
        if (!connected_)
        {
            std::cerr << "Not connected. Cannot receive message." << std::endl;
            return "";
        }

        char buffer[1024];
        std::size_t bytesReceived = socket_.read_some(asio::buffer(buffer));

        return std::string(buffer, bytesReceived);
    }

    void disconnect()
    {
        if (!connected_)
        {
            std::cerr << "Not connected." << std::endl;
            return;
        }

        socket_.shutdown();
        connected_ = false;
        std::cout << "Disconnected from the sender" << std::endl;
    }

private:
    bool connected_;

    void handleConnect(const boost::system::error_code &error)
    {
        if (!error)
        {
            std::cout << "Connected to the sender" << std::endl;
        }
        else
        {
            std::cout << "Connect error: " << error.message() << std::endl;
        }
    }

    asio::io_service io_service_;
    ssl::context ctx_;
    ssl::stream<tcp::socket> socket_;
};
