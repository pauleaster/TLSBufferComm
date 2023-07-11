#include <iostream>
#include "receiver.cpp"

int main() {
    std::string certificateEnvVar = "EB_RECEIVER_CERTIFICATE_DATA";
    std::string privateKeyEnvVar = "EB_RECEIVER_PRIVATE_KEY_DATA";

    try {
        Receiver receiver(certificateEnvVar, privateKeyEnvVar);
        receiver.connect();

        std::string msg1 = receiver.receive();
        std::cout << "Received message: " << msg1 << std::endl;
        receiver.disconnect();
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
