#include <iostream>
#include "client.cpp"

int main() {
    std::string message = "Hello from client";
    std::string certificateEnvVar = "EB_CLIENT_CERTIFICATE_DATA";
    std::string privateKeyEnvVar = "EB_CLIENT_PRIVATE_KEY_DATA";

    try {
        Client client(certificateEnvVar, privateKeyEnvVar);
        client.connect("127.0.0.1", 4321);
        client.send(message);
        client.disconnect();
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
