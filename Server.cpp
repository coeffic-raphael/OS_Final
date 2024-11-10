#include "Server.hpp"
#include "CommandHandler.hpp"
#include <cstring>
#include <arpa/inet.h>

Server::Server(int port) : port(port) {
    // Initialize serverRunning flag to true
    serverRunning = std::make_shared<std::atomic<bool>>(true);
}

Server::~Server() {
    // Ensure the server stops when the object is destroyed
    stop();
}

void Server::start() {
    // Check if the server is already stopped
    if (!serverRunning->load()) {
        std::cout << "Server is already stopped." << std::endl;
        return;
    }

    // Create a socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }

    sockaddr_in address;
    int opt = 1;
    
    // Set socket options to reuse the address
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // Bind the socket to the specified port
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections (maximum 3 in the queue)
    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server started on port " << port << std::endl;

    // Main server loop
    while (serverRunning->load()) {
        sockaddr_in client_address;
        socklen_t client_addrlen = sizeof(client_address);
        
        // Accept incoming connections
        int client_fd = accept(server_fd, (struct sockaddr*)&client_address, &client_addrlen);
        if (client_fd < 0) {
            if (serverRunning->load()) {
                perror("Failed to accept");
            }
            continue;
        }

        // Launch a new thread to handle the client connection
        std::thread client_thread(&Server::handleClient, this, client_fd);
        
        
        {
            std::lock_guard<std::mutex> lock(threads_mutex);
            threads.emplace_back(std::move(client_thread));
        }
    }

    // Stop the server when the loop ends
    stop();
}


void Server::stop() {
    std::call_once(stop_flag, [this]() {
        {
            std::lock_guard<std::mutex> lock(coutMutex);
            serverRunning->store(false); // Signal server to stop running
        }
        close(server_fd); // Close the server socket

        // Join all the threads of the vector with security
        {
            std::lock_guard<std::mutex> lock(threads_mutex);
            for (auto& t : threads) {
                if (t.joinable()) {
                    t.join(); // Ensure all client threads are joined
                }
            }
            
            threads.clear();
        }
        
        std::cout << "Server stopped." << std::endl;
    });
}

void Server::handleClient(int client_fd) {
    char buffer[1024] = {0};
    std::string clientMessage;
    CommandHandler<double> commandHandler(serverRunning);

    // Client handling loop
    while (serverRunning->load()) {
        // Read data from the client
        ssize_t valread = read(client_fd, buffer, sizeof(buffer));
        if (valread <= 0) {
            break;  // Stop reading if the client disconnects or an error occurs
        }

        // Convert the buffer to a string and clean up newlines
        clientMessage = std::string(buffer, valread);
        clientMessage.erase(std::remove(clientMessage.begin(), clientMessage.end(), '\n'), clientMessage.end());

        // Handle the client's command
        commandHandler.handleCommand(client_fd, clientMessage);

        // If the server is instructed to stop, break the loop
        if (!serverRunning->load()) {
            break;
        }
    }

    // Close the connection with the client
    close(client_fd);
}