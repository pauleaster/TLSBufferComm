#include "../include/server.hpp"

int main()
{
    const std::string certificateEnvVar = "EB_SERVER_CERTIFICATE_DATA";
    const std::string privateKeyEnvVar = "EB_SERVER_PRIVATE_KEY_DATA";
    const std::string serverIP = "127.0.0.1"; // Use the loopback address (localhost)
    const unsigned short port = 4321;

    Server rv(certificateEnvVar, privateKeyEnvVar, serverIP, port);
    std::string message = rv.run();
    std::cout << "Received message: \n\"" << message << "\"\n"
              << std::endl;

    return 0;
}