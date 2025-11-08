#pragma once
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

// -----------------------------------------------------------------------------
// HiveMnd Utility Library : ThreadPool
// Purpose: Manages a pool of worker threads that execute queued tasks.
// Notes  : Supports dynamic scaling and graceful shutdown.
// -----------------------------------------------------------------------------
namespace HiveMnd::Core
{
    class ThreadPool
    {
    private:
        std::vector<std::thread> mWorkers;               // All active worker threads
        std::queue<std::function<void()>> mTasks;        // Task queue
        std::mutex mQueueMutex;                          // Protects access to task queue
        std::condition_variable mCondition;              // Signals threads when tasks are available
        std::atomic<bool> mStop;                         // Signals shutdown

    public:
        // ---------------------------------------------------------------------
        //  Constructor: ThreadPool
        //  Purpose : Initializes and spawns worker threads.
        // ---------------------------------------------------------------------
        explicit ThreadPool(size_t threadCount = std::thread::hardware_concurrency());

        // ---------------------------------------------------------------------
        //  Function: Enqueue
        //  Purpose : Adds a new task to the thread pool.
        // ---------------------------------------------------------------------
        void Enqueue(std::function<void()> task);

        // ---------------------------------------------------------------------
        //  Function: Shutdown
        //  Purpose : Gracefully stops all worker threads.
        // ---------------------------------------------------------------------
        void Shutdown();

        // ---------------------------------------------------------------------
        //  Destructor
        //  Purpose : Ensures proper cleanup of threads.
        // ---------------------------------------------------------------------
        ~ThreadPool();
    };
}

