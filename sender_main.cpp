#define BOOST_ASIO_ENABLE_HANDLER_TRACKING 2
#include <iostream>
#include "sender.cpp"

int main() {
    std::string message = "Hello from sender";
    std::string certificateEnvVar = "EB_SENDER_CERTIFICATE_DATA";
    std::string privateKeyEnvVar = "EB_SENDER_PRIVATE_KEY_DATA";

    try {
        Sender sender(certificateEnvVar, privateKeyEnvVar);
        sender.connect("127.0.0.1", 1234);
        sender.send(message);
        sender.disconnect();
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
