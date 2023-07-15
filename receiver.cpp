#define BOOST_ASIO_ENABLE_HANDLER_TRACKING 2
#include <iostream>
#include <array>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

namespace asio = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = asio::ip::tcp;

class Receiver
{
public:
    Receiver(const std::string &certificateEnvVar, const std::string &privateKeyEnvVar)
        : io_context_(),
          ctx_(ssl::context::sslv23_server),
          sslSocket_(io_context_, ctx_),
          acceptor_(io_context_),
          connected_(false)
    {

        // read certificate and private key from environment variables
        const char *certificateData = std::getenv(certificateEnvVar.c_str());
        const char *privateKeyData = std::getenv(privateKeyEnvVar.c_str());
        if (!certificateData || !privateKeyData)
        {
            std::cerr << "Receiver certificate data or private key data not provided." << std::endl;
            throw std::runtime_error("Certificate or private key data missing.");
        }

        ctx_.use_certificate(asio::buffer(certificateData, std::strlen(certificateData)),
                             ssl::context::pem,
                             certificateError);
        ctx_.use_private_key(asio::buffer(privateKeyData, std::strlen(privateKeyData)),
                             ssl::context::pem,
                             privateKeyError);
        // Set the certificate verification mode to SSL_VERIFY_PEER,
        // which means the client will verify the server's certificate.
        ctx_.set_verify_mode(ssl::verify_peer);
        if (certificateError)
        {
            auto certificateError_ = certificateError.message();
            std::cout << "Failed to load the certificate: " << certificateError_ << std::endl;
        }

        if (privateKeyError)
        {
            auto privateKeyError_ = privateKeyError.message();
            std::cout << "Failed to load the private key: " << privateKeyError_ << std::endl;
        }
        std::cout << "Initialisation: Checking Certificate and Private Key strings..." << std::endl;
        checkCertificate();
        checkPrivateKey();
    }

    void startAccept()
    {
        acceptor_.async_accept(
            sslSocket_.lowest_layer(),
            [this](const boost::system::error_code &error)
            {
                if (!error)
                {
                    std::cout << "Connection accepted" << std::endl;
                    connected_ = true;

                    std::cout << "Checking Certificate and Private Key strings..." << std::endl;
                    checkCertificate();
                    checkPrivateKey();

                    sslSocket_.async_handshake(
                        ssl::stream_base::server,
                        [this](const boost::system::error_code &handshakeError)
                        {
                            if (!handshakeError)
                            {
                                std::cout << "Handshake successful" << std::endl;
                                // Handle further communication with the client, if needed
                            }
                            else
                            {
                                std::cout << "Handshake failed: " << handshakeError.message() << std::endl;
                                // Handle the handshake error as needed

                                // Close the sslSocket_
                                boost::system::error_code closeError;
                                std::cout << "Closing the socket..." << std::endl;
                                sslSocket_.lowest_layer().close(closeError);
                                if (closeError)
                                {
                                    std::cout << "Failed to close the socket: " << closeError.message() << std::endl;
                                    // Handle the socket close error as needed
                                }

                                // Set connected_ to false since the connection is no longer established
                                connected_ = false;
                                std::cout << "Connection closed" << std::endl;
                            }
                        });
                }
                else
                {
                    std::cout << "Connection error: " << error.message() << std::endl;
                    // Handle the connection error as needed
                }
            });
    }

    void connect(const std::string &receiverIP, unsigned short port)
    {
        tcp::endpoint endpoint(asio::ip::address::from_string(receiverIP), port);
        acceptor_.open(endpoint.protocol());
        acceptor_.set_option(tcp::acceptor::reuse_address(true));
        acceptor_.bind(endpoint);
        acceptor_.listen();

        std::cout << "Listening for incoming connections..." << std::endl;
        startAccept();
    }

    std::string receive()
    {
        std::array<char, 128> buffer;
        boost::system::error_code error;
        size_t bytesRead = sslSocket_.read_some(boost::asio::buffer(buffer), error);
        io_context_.run();

        if (!connected_)
        {
            std::cout << "Not connected, aborting receive" << std::endl;
            return "";
        }
        else
        {
            std::cout << "Connected, receiving..." << std::endl;
        }
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
            return std::string(buffer.data(), bytesRead);
        }

        return "";
    }

    void disconnect()
    {
        if (connected_)
        {
            std::cout << "Disconnecting..." << std::endl;
            sslSocket_.shutdown();
        }
        else
        {
            std::cout << "Not connected, aborting disconnect" << std::endl;
        }
    }

private:
    asio::io_context io_context_;
    ssl::context ctx_;
    tcp::acceptor acceptor_;
    ssl::stream<tcp::socket> sslSocket_;
    bool connected_;
    boost::system::error_code certificateError;
    boost::system::error_code privateKeyError;
    boost::system::error_code handshakeError;

    void checkCertificate()
    {
        // Check the certificate status
        long verifyMode = SSL_CTX_get_verify_mode(ctx_.native_handle());
        if (verifyMode & SSL_VERIFY_PEER)
        {
            std::cout << "Certificate verification is enabled" << std::endl;
        }
        else
        {
            std::cout << "Certificate verification is disabled" << std::endl;
        }

        // use SSL_CTX_get0_certificate to get certificate from ctx_
        X509 *certificate = SSL_CTX_get0_certificate(ctx_.native_handle());
        if (certificate != nullptr)
        {
            std::cout << "Certificate is available" << std::endl;
            // convert X509 to string
            std::string certificateString;
            BIO *bio = BIO_new(BIO_s_mem());
            if (bio != nullptr)
            {
                if (PEM_write_bio_X509(bio, certificate) == 1)
                {
                    char buffer[1024];
                    int bytesRead;
                    while ((bytesRead = BIO_read(bio, buffer, sizeof(buffer))) > 0)
                    {
                        certificateString.append(buffer, bytesRead);
                    }
                }
            }
            BIO_free_all(bio);
            // Output the certificate string
            std::cout << "Certificate: \n"
                      << certificateString.substr(28, 28 + 10) << std::endl;
        }
        else
        {
            std::cout << "Certificate is not available" << std::endl;
        }
    }

    void checkPrivateKey()
    {
        // Use SSL_CTX_get0_privatekey to get the private key from ctx_
        EVP_PKEY *privateKey = SSL_CTX_get0_privatekey(ctx_.native_handle());
        if (privateKey != nullptr)
        {
            std::cout << "Private key is available" << std::endl;

            // Convert EVP_PKEY to string
            std::string privateKeyString;
            BIO *bio = BIO_new(BIO_s_mem());
            if (bio != nullptr)
            {
                if (PEM_write_bio_PrivateKey(bio, privateKey, nullptr, nullptr, 0, nullptr, nullptr) == 1)
                {
                    char buffer[1024];
                    int bytesRead;
                    while ((bytesRead = BIO_read(bio, buffer, sizeof(buffer))) > 0)
                    {
                        privateKeyString.append(buffer, bytesRead);
                    }
                }
                BIO_free_all(bio);
            }

            // Output the private key string
            std::cout << "Private key: \n"
                      << privateKeyString.substr(28, 28 + 10) << std::endl;
        }
        else
        {
            std::cout << "Private key is not available" << std::endl;
        }
    }
};
