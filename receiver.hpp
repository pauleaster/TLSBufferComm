// receiver.hpp (Header file for Receiver class)
#pragma once

#include <array>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <optional>
#include <string>
#include <iostream>

namespace asio = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = asio::ip::tcp;

class Receiver
{
public:
  Receiver(const std::string &certificateEnvVar, const std::string &privateKeyEnvVar, const std::string receiverIP, const unsigned short port);

  std::string run();
  void startListening();
  void acceptConnection();
  void initialiseSSL();
  void doHandshake();
  std::string receiveData();
  void closeSocket();

private:
  asio::io_context io_context;
  ssl::context ctx;
  std::string certificateData;
  std::string privateKeyData;
  tcp::endpoint endpoint;
  tcp::acceptor acceptor;
  std::shared_ptr<tcp::socket> socket;               // Using shared_ptr for socket
  std::optional<ssl::stream<tcp::socket>> sslSocket; // Use std::optional for ssl::stream
  std::array<char, 128> buffer;
  boost::system::error_code error;
  size_t bytesRead;
  std::string msg;
  std::string receiverIP_;
  unsigned short port_;

  std::string getEnvVariable(const std::string &varName);
};
