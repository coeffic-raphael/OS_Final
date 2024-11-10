#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP

#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <atomic>

class ThreadPool {
private:
    // Vector of worker threads
    std::vector<std::thread> workers;

    // Mutex for synchronization
    std::mutex mutex;

    // Condition variable for notification
    std::condition_variable condition;

    // Queue of tasks
    std::queue<std::function<void()>> tasks;

    // Flag to stop the thread pool
    std::atomic<bool> stopFlag;

    // Flag to indicate if a leader is currently assigned
    bool leaderAssigned;

public:
    // Constructor
    ThreadPool(size_t numThreads);

    // Destructor
    ~ThreadPool();

    // Method to add a task
    void enqueue(std::function<void()> task);

    // Method to stop the thread pool
    void stop();
};

#endif // THREADPOOL_HPP