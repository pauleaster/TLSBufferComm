// client.cpp
#include "client.hpp"

Client::Client(const std::string &certificateEnvVar, const std::string &privateKeyEnvVar)
    : io_context_(),
      ctx_(ssl::context::tlsv13_client),
      socket_(io_context_, ctx_),
      connected_(false)
{
    const char *certificateData = std::getenv(certificateEnvVar.c_str());
    const char *privateKeyData = std::getenv(privateKeyEnvVar.c_str());

    if (!certificateData || !privateKeyData)
    {
        std::cerr << "Client certificate data or private key data not provided." << std::endl;
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

void Client::connect(const std::string &serverIP, unsigned short port)
{
    if (connected_)
    {
        std::cerr << "Already connected." << std::endl;
        return;
    }

    tcp::resolver resolver(io_context_);
    tcp::resolver::query query(serverIP, std::to_string(port));
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
                    ssl::stream_base::client,
                    [this](const boost::system::error_code &error)
                    {
                        if (!error)
                        {
                            connected_ = true;
                            std::cout << "Connected to the server" << std::endl;
                        }
                        else
                        {
                            std::cout << "Handshake error: " << error.message() << std::endl;
                        }
                    });
            }
        });

    io_context_.run();
}

void Client::send(const std::string &message)
{
    if (!connected_)
    {
        std::cerr << "Not connected. Cannot send message." << std::endl;
        return;
    }

    asio::async_write(
        socket_,
        asio::buffer(message),
        [this](const boost::system::error_code &error, size_t bytesTransferred)
        {
            handleWrite(error, bytesTransferred);
        });
}

void Client::disconnect()
{
    if (!connected_)
    {
        std::cerr << "Not connected." << std::endl;
        return;
    }

    socket_.shutdown();
    connected_ = false;
    std::cout << "Disconnected from the server" << std::endl;
}

void Client::handleConnect(const boost::system::error_code &error)
{
    if (!error)
    {
        std::cout << "Connected to the server" << std::endl;
    }
    else
    {
        std::cout << "Connect error: " << error.message() << std::endl;
    }
}

void Client::handleWrite(const boost::system::error_code &error, size_t bytesTransferred)
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

void Client::checkCertificate()
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

void Client::checkPrivateKey()
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
