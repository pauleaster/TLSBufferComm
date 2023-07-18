#include "receiver.cpp"

int main()
{
    const std::string certificateEnvVar = "EB_RECEIVER_CERTIFICATE_DATA";
    const std::string privateKeyEnvVar = "EB_RECEIVER_PRIVATE_KEY_DATA";

    Receiver rv(certificateEnvVar, privateKeyEnvVar);
    rv.run();

    return 0;
}