#ifndef POOL_H 
#define POOL_H

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>


class ThreadPool {
    public: 

        // Constructor. This does not start the pool, but only creates an instance of this 
        // class with an assigned number of workers. The `start` function is added to 
        // allow defining a set of jobs before actually starting them.
        ThreadPool(size_t nThreads);

        // Destructor to stop the thread pool
        ~ThreadPool();

        // Start the Pool by creating all the working threads
        void start();

        // Enqueue a task to be executed by the thread pool
        void addTask(std::function<void()> task);

    private: 

        // Number of workers available to the pool
        size_t nThreads;

        // List of worker threads
        std::vector<std::thread> workers; 
        // Queue storing all the jobs that need to be performed. 
        std::queue<std::function<void()>> tasks; 
        // Mutex to synchronize access to all the shared data.
        std::mutex queueMutex; 
        // Condition variable to notify workers
        std::condition_variable cv; 

        // Flag that indicates whether the thread-pool should be stopped.
        bool stop = false;
        // Flag that indicates whether the pool should start processing the jobs. This is 
        // done so that we have the capability to add a certain number of tasks before 
        // starting to process them.
        bool started = false;

        void workerLoop();
};

#endif