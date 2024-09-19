#ifndef POOL_H 
#define POOL_H

#include "types.h"

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

/**
 * @class ThreadWorker
 * @brief Cache class storing information on a specific thread.
 */
class ThreadWorker {

    public: 
    
        /**
         * @brief Construct a new Thread Worker object given its ID.
         * 
         * @param id Thread ID.
         */
        ThreadWorker(ui32_t id); 

        /**
         * @brief Return the thread's ID.
         * @return ui32_t Thread ID.
         */
        ui32_t id() const; 

    private: 
        ui32_t _id; 

};


/**
 * @class ThreadPool 
 * @brief Class representing a pool of threads.
 * 
 * @details This class provides a pool of threads that can be used to execute tasks, post 
 * work items, process asynchronous tasks and wait on behalf of other threads. 
 */
class ThreadPool {
    public: 

        /**
         * @brief Construct a new Thread Pool object with a given amount of threads.
         * 
         * @param nThreads Number of desired threads.
         * @note Creating a ThreadPool object does not automatically start the pool, but 
         * only creates an instance of this class with an assigned number of workers. This 
         * allows defining a set of jobs before actually starting them.
         */
        ThreadPool(size_t nThreads);

        // Destructor to stop the thread pool
        ~ThreadPool();

        /**
         * @brief Return the number of threads in the pool.
         * @return size_t Number of threads.
         */
        size_t nThreads() const; 

        /**
         * @brief Return the number of remaining tasks, i.e., those that are yet to be 
         * completed.
         * @return size_t Number of pending tasks.
         */
        size_t nPendingTasks() const; 

        /**
         * @brief Start the pool. 
         * @details This creates all the working threads and makes them ready to pick-up 
         * any assigned tasks.
         */
        void startPool();
    
        /**
         * @brief Stop the pool. 
         * @details Before stopping, this function will wait for the completion of all active 
         * tasks. All the tasks that are still waiting in the queue will not be executed.
         */
        void stopPool(); 

        /**
         * @brief Check whether the thread pool has been started.
         */
        bool isRunning(); 

        /**
         * @brief Check whether all the tasks have been executed.
         */
        bool isBusy(); 

        /**
         * @brief Puts the calling thread on hold until all the tasks in the queue have 
         * been completed.
         */
        void waitCompletion();

        /**
         * @brief Queue a task for execution.
         * @param task Task function.
         */
        void addTask(std::function<void(const ThreadWorker&)> task);

    private: 

        // Number of workers available to the pool
        size_t _nThreads;

        // List of worker threads
        std::vector<std::thread> workers; 
        // Queue storing all the jobs that need to be performed. 
        std::queue<std::function<void(const ThreadWorker&)>> tasks; 

        // Mutex to synchronize access to all the shared data.
        std::mutex queueMutex; 
        // Condition variable to notify workers
        std::condition_variable task_cv; 
        
        // Number of pending/uncompleted tasks
        std::atomic<size_t> pendingTasks = 0;
        // Mutex and condition to wait for the task completion
        std::mutex waitMutex;
        std::condition_variable wait_cv; 

        // Flag that indicates whether the thread-pool should be stopped.
        bool stop = false;
        // Flag that indicates whether the pool should start processing the jobs. This is 
        // done so that we have the capability to add a certain number of tasks before 
        // starting to process them.
        bool started = false;

        void workerLoop(ThreadWorker wk);
};

#endif