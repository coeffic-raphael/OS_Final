#ifndef ACTIVE_OBJECT_HPP
#define ACTIVE_OBJECT_HPP

#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <functional>
#include <memory>
#include <atomic>
#include "Message.hpp"

template<typename T>
class ActiveObject {
private:
    std::thread worker; // Worker thread that processes tasks
    std::queue<std::pair<std::shared_ptr<Message<T>>, std::function<void(std::shared_ptr<Message<T>>)>> > tasks; // Task queue containing pairs (message, function)
    std::mutex mtx; // Mutex to protect access to the task queue
    std::condition_variable cv; // Condition variable to notify the worker of new tasks
    std::atomic<bool> running; // Atomic flag to control the state of the worker

public:
    // Default constructor
    ActiveObject();

    // Destructor that ensures the worker is stopped
    ~ActiveObject();

    // Method to send a new task to the active object
    void send(std::shared_ptr<Message<T>> msg, std::function<void(std::shared_ptr<Message<T>>)> func);

    // Method to stop the active object and its worker thread
    void stop();
};

#endif // ACTIVE_OBJECT_HPP