#ifndef COMMANDHANDLER_HPP
#define COMMANDHANDLER_HPP

#include <string>
#include <sstream>
#include <vector>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include "Graph.hpp"
#include "MSTFactory.hpp"
#include "ActiveObject.hpp"
#include "ThreadPool.hpp"
#include "Message.hpp"
#include <unordered_map>

template<typename T>
class CommandHandler {
private:
    // Shared flag to control whether the server is running
    std::shared_ptr<std::atomic<bool>> serverRunning;

    // Active Objects for parsing commands, computing MST, and calculating metrics
    ActiveObject<T> parserAO;
    ActiveObject<T> mstAO;
    ActiveObject<T> metricsAO;

    // ThreadPool for handling tasks in Leader-Follower pattern
    ThreadPool threadPool;

    // Mutex to protect access to the client graph data
    std::mutex graphMutex;

    // Map to store the graph for each client, identified by their file descriptor
    std::unordered_map<int, std::unique_ptr<Graph<T>>> clientGraphs;

public:
    // Constructor: Initializes the command handler with the server's running state and thread pool size
    CommandHandler(std::shared_ptr<std::atomic<bool>> serverRunning, size_t numThreads = 4);

    // Destructor: Ensures that all resources are properly cleaned up
    ~CommandHandler();

    // Main method to handle incoming commands from a client
    void handleCommand(int client_fd, const std::string& command);

private:
    // Sends a response back to the client based on the command's result
    void sendResponse(std::shared_ptr<Message<T>> msg);
};

#endif // COMMANDHANDLER_HPP   
