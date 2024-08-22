
#include "pool.h" 

// Constructor. This does not start the pool, but only creates an instance of this 
// class with an assigned number of workers. The `start` function is added to 
// allow defining a set of jobs before actually starting them.
ThreadPool::ThreadPool(size_t nThreads) : nThreads(nThreads) {}

// Destructor to stop the thread pool
ThreadPool::~ThreadPool() { stopPool(); }

// Start the Pool by creating all the working threads
void ThreadPool::startPool()
{

    if (isRunning()) {
        return; 
    }

    {
        std::unique_lock<std::mutex> lock(queueMutex); 
        started = true; 
        stop = false;
    }

    for (size_t k = 0; k < nThreads; k++)
        workers.emplace_back(std::thread(&ThreadPool::workerLoop, this));
}

// Stop the Pool. This will wait completion of all active tasks and then stop.
void ThreadPool::stopPool()
{
    {
        std::unique_lock<std::mutex> lock(queueMutex); 
        stop = true; 
        started = false;
    }

    cv.notify_all();
    
    // Joining all worker threads to ensure the have completed their tasks 
    for (std::thread& worker : workers) {
        worker.join();
    }

    workers.clear();

}

// Check whether the pool was started (i.e., workers were assigned )    
bool ThreadPool::isRunning() 
{ 
    bool run; 
    {
        std::unique_lock<std::mutex> lock(queueMutex); 
        run = started;
    }
    return run;
}

// Check whether any of the workers in the pool are performing some operations.
bool ThreadPool::isBusy()
{
    bool busy;
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        busy = !tasks.empty();
    }
    return busy;
}


// Enqueue a task to be executed by the thread pool
void ThreadPool::addTask(std::function<void()> task)
{
    {
        std::unique_lock<std::mutex> lock(queueMutex); 
        tasks.emplace(std::move(task)); 
    }

    cv.notify_one();
}   


void ThreadPool::workerLoop() {
    // The while loop keeps iterating to keep the thread alive. Only when the 
    // thread-pool is effectively stopped and the task list is empty the function 
    // is closed.
    while (true) {
        std::function<void()> task; 
        {
            // Lock the queue to allow data to be shared safely
            std::unique_lock<std::mutex> lock(queueMutex); 
            // Wait until there is a task to execute or the pool is stopped
            cv.wait(lock, [this] { return (!tasks.empty() || stop); });

            // If the pool has been stopped and there are no tasks, exit 
            if (stop && tasks.empty()) {
                return; 
            }

            // Get the next available task from the queue 
            task = std::move(tasks.front()); 
            tasks.pop(); 

        }

        // Execute the task
        task(); 
    }
}

