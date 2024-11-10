#include "ThreadPool.hpp"

ThreadPool::ThreadPool(size_t numThreads) : stopFlag(false), leaderAssigned(false) {
    // Create the specified number of worker threads
    for (size_t i = 0; i < numThreads; ++i) {
        workers.emplace_back([this]() {
            while (true) {
                std::function<void()> task;

                {
                    std::unique_lock<std::mutex> lock(this->mutex);

                    // Wait until a task is available or the pool is stopped
                    // Also wait if a leader has already been assigned
                    while (!stopFlag && (tasks.empty() || leaderAssigned)) {
                        condition.wait(lock); // Wait for tasks or stop signal
                    }

                    // If the pool is stopped and there are no tasks, exit the thread
                    if (stopFlag && tasks.empty())
                        return;

                    // Assign the current thread as the leader
                    leaderAssigned = true;

                    // The leader takes the task from the queue
                    task = std::move(tasks.front());
                    tasks.pop();
                }

                // Execute the task
                task();

                // Transfer the leadership role
                {
                    std::unique_lock<std::mutex> lock(this->mutex);

                    // Check if there are still tasks left to process
                    if (!tasks.empty()) {
                        // Release leadership and notify another thread to become the new leader
                        leaderAssigned = false;
                        condition.notify_one(); // Notify another thread to take over as leader
                    } else {
                        // If no tasks remain, release leadership
                        leaderAssigned = false;
                    }
                }
            }
        });
    }
}

void ThreadPool::enqueue(std::function<void()> task) {
    {
        std::lock_guard<std::mutex> lock(mutex);

        // Prevent adding tasks to a stopped thread pool
        if (stopFlag)
            throw std::runtime_error("Enqueue on a stopped ThreadPool");

        // Add the new task to the task queue
        tasks.push(std::move(task));

        // If no leader is currently assigned, notify a thread to become the leader
        if (!leaderAssigned) {
            condition.notify_one();
        }
    }
}

void ThreadPool::stop() {
    {
        std::lock_guard<std::mutex> lock(mutex);
        stopFlag = true;  // Signal to stop the thread pool
        condition.notify_all(); // Wake up all threads so they can exit
    }

    // Join all worker threads, waiting for them to finish
    for (std::thread &worker : workers) {
        if (worker.joinable())
            worker.join();
    }
}

ThreadPool::~ThreadPool() {
    stop();  // Ensure the pool is stopped when destroyed
}