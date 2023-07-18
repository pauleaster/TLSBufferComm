#include "receiver.cpp"

int main()
{
    const std::string certificateEnvVar = "EB_RECEIVER_CERTIFICATE_DATA";
    const std::string privateKeyEnvVar = "EB_RECEIVER_PRIVATE_KEY_DATA";

    Receiver rv(certificateEnvVar, privateKeyEnvVar);
    std::string message = rv.run();
    std::cout << "Received message: \n\"" << message << "\"\n" << std::endl;

    return 0;
}