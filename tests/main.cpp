#include <iostream>
#include <thread>
#include <condition_variable>
#include "../include/server.hpp"
#include "../include/client.hpp"

std::condition_variable cv;
std::mutex cv_m; // This mutex is used for the condition variable
bool server_started = false;
std::string client_message;
std::mutex message_mutex;

void client_thread_function() {
    {
        std::lock_guard<std::mutex> lk(message_mutex);
        client_message = "Hello from client";
    }
    
    std::string certificateEnvVar = "EB_CLIENT_CERTIFICATE_DATA";
    std::string privateKeyEnvVar = "EB_CLIENT_PRIVATE_KEY_DATA";

    // Wait until the server has started
    {
        std::unique_lock<std::mutex> lk(cv_m);
        cv.wait(lk, []{ return server_started; });
    }

    try {
        Client client(certificateEnvVar, privateKeyEnvVar);
        client.connect("127.0.0.1", 4321);
        client.send(client_message);
        client.disconnect();
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

int main() {
    const std::string certificateEnvVar = "EB_SERVER_CERTIFICATE_DATA";
    const std::string privateKeyEnvVar = "EB_SERVER_PRIVATE_KEY_DATA";
    const std::string serverIP = "127.0.0.1"; // Use the loopback address (localhost)
    const unsigned short port = 4321;

    Server rv(certificateEnvVar, privateKeyEnvVar, serverIP, port);

    // Start client thread
    std::thread client_thread(client_thread_function);

    // Notify the client that the server has started
    {
        std::lock_guard<std::mutex> lk(cv_m);
        server_started = true;
    }
    cv.notify_one();

    // The server process runs in the main thread
    std::string server_message = rv.run();

    // Wait for client thread to finish
    client_thread.join();

    // Check if messages are the same
    if(server_message == client_message) {
        // The server and client messages are the same.
        return 0; // success
    } else {
        // The server and client messages are not the same.\n
        return 1; // failure
    }
}
