#ifndef POOL_H 
#define POOL_H

#include <atomic>
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
        void startPool();
    
        // Stop the Pool. This will wait completion of all active tasks and then stop.
        void stopPool(); 

        // Check whether the pool was started (i.e., workers were assigned )    
        bool isRunning(); 
    
        // Check whether all the jobs have been completed
        bool isBusy(); 


        // Wait until all the tasks are completed
        void waitCompletion();

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
        std::condition_variable task_cv; 
        
        // Number of pending/uncompleted tasks
        std::atomic<int> pendingTasks = 0;
        // Mutex and condition to wait for the task completion
        std::mutex waitMutex;
        std::condition_variable wait_cv; 

        // Flag that indicates whether the thread-pool should be stopped.
        bool stop = false;
        // Flag that indicates whether the pool should start processing the jobs. This is 
        // done so that we have the capability to add a certain number of tasks before 
        // starting to process them.
        bool started = false;

        void workerLoop();
};

#endif