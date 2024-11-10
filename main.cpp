#include "Server.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    int port = 1111; // Default port

    // If a port number is provided as a command-line argument, use it
    if (argc > 1) {
        port = std::stoi(argv[1]); // Convert the argument to an integer for the port
    }

    // Create a Server object with the specified port
    Server server(port);
    
    // Start the server to listen for incoming connections and handle commands
    server.start();    

    return 0; // Exit the program
}
