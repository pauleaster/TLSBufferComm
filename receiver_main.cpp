#include "receiver.cpp"

int main()
{
    const std::string certificateEnvVar = "EB_RECEIVER_CERTIFICATE_DATA";
    const std::string privateKeyEnvVar = "EB_RECEIVER_PRIVATE_KEY_DATA";
    const std::string receiverIP = "127.0.0.1"; // Use the loopback address (localhost)
    const unsigned short port = 4321;

    Receiver rv(certificateEnvVar, privateKeyEnvVar, receiverIP, port);
    std::string message = rv.run();
    std::cout << "Received message: \n\"" << message << "\"\n"
              << std::endl;

    return 0;
}