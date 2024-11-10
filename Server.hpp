#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <mutex>
#include <memory>
#include <netinet/in.h>
#include <unistd.h>

class Server {
private:
    int server_fd;  // File descriptor for the server socket
    int port;  // Port on which the server listens
    std::shared_ptr<std::atomic<bool>> serverRunning;  // Atomic flag to control server running state
    std::vector<std::thread> threads;  // Vector to store client-handling threads
    std::mutex threads_mutex;
    std::mutex coutMutex;  // Mutex to synchronize console output

public:

    std::once_flag stop_flag;

    // Constructor to initialize the server with the specified port
    Server(int port);

    // Destructor to ensure proper shutdown of the server
    ~Server();

    // Start the server (listens for and accepts incoming connections)
    void start();

    // Stop the server (closes the socket and stops client handling)
    void stop();

private:
    // Handles communication with a connected client
    void handleClient(int client_fd);
};

#endif // SERVER_HPP 