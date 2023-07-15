#define BOOST_ASIO_ENABLE_HANDLER_TRACKING 2
#include "receiver.cpp"

const std::string certificateEnvVar = "EB_RECEIVER_CERTIFICATE_DATA";
const std::string privateKeyEnvVar = "EB_RECEIVER_PRIVATE_KEY_DATA";

int main() {

    Receiver receiver(certificateEnvVar, privateKeyEnvVar);
    receiver.connect("127.0.0.1", 1234);
    std::string msg = receiver.receive();
    receiver.disconnect();

    return 0;
}
