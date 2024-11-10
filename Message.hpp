#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <string>
#include <memory>
#include <mutex>
#include <condition_variable>
#include "Graph.hpp"
#include "Tree.hpp"

template<typename T>
struct Message {
    int client_fd; // File descriptor of the client for sending responses
    std::string rawCommand; // The raw command received from the client
    std::string cmd; // Parsed command
    std::string algo; // Algorithm for MST (e.g., "Prim", "Kruskal")
    std::string pattern; // Design pattern to use (e.g., "Pipeline", "Leader-Follower")
    std::string response; // Response to send back to the client
    std::unique_ptr<Tree<T>> mst; // Calculated Minimum Spanning Tree (MST)
    bool error; // Indicates if there was an error during command execution
    std::string errorMessage; // Stores the error message if an error occurred
};

#endif // MESSAGE_HPP

