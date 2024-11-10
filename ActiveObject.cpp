#include "ActiveObject.hpp"

template<typename T>
ActiveObject<T>::ActiveObject() : running(true) {
    // Start the worker thread
    worker = std::thread([this]() {
        while (true) {
            // Declare a task that consists of a message and its associated function
            std::pair<std::shared_ptr<Message<T>>, std::function<void(std::shared_ptr<Message<T>>)>> task;
            {
                // Lock the mutex to protect access to the task queue
                std::unique_lock<std::mutex> lock(this->mtx);
                
                // Wait until there are tasks in the queue or until we receive a stop signal
                this->cv.wait(lock, [this]() { return !this->tasks.empty() || !running; });
                
                // If running is false and the task queue is empty, exit the loop
                if (!running && this->tasks.empty()) {
                    return;
                }
                
                // Extract the next task from the queue
                task = std::move(this->tasks.front());
                this->tasks.pop();
            }
            
            // Execute the task using the provided function
            task.second(task.first);
        }
    });
}

template<typename T>
ActiveObject<T>::~ActiveObject() {
    // Stop the worker thread when the object is destroyed
    stop();
}

template<typename T>
void ActiveObject<T>::send(std::shared_ptr<Message<T>> msg, std::function<void(std::shared_ptr<Message<T>>)> func) {
    {
        // Lock the mutex to safely add a task to the queue
        std::lock_guard<std::mutex> lock(mtx);
        
        // Add the new task (message, function) to the task queue
        tasks.emplace(msg, func);
        
        // Notify one waiting thread (the worker) that a new task is available
        cv.notify_one();
    }
}

template<typename T>
void ActiveObject<T>::stop() {
    {
        // Lock the mutex and set running to false, signaling the worker to stop
        std::lock_guard<std::mutex> lock(mtx);
        running = false;
        
        // Notify all threads, including the worker waiting for tasks
        cv.notify_all();
    }
    
    // If the worker thread is joinable, wait for it to finish its execution
    if (worker.joinable()) {
        worker.join();
    }
}

// Explicit instantiation of the template for the type double
template class ActiveObject<double>;