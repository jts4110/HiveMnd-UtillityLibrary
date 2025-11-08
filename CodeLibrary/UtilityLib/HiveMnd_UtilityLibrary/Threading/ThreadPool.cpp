#include "ThreadPool.h"
#include <iostream>

namespace HiveMnd::Core
{
    // -------------------------------------------------------------------------
    // Constructor: Initializes worker threads and main loop
    // -------------------------------------------------------------------------
    ThreadPool::ThreadPool(size_t threadCount) : mStop(false)
    {
        if (threadCount == 0)
            threadCount = 1;

        for (size_t i = 0; i < threadCount; ++i)
        {
            mWorkers.emplace_back([this, i]()
                {
                    while (true)
                    {
                        std::function<void()> task;

                        // Wait for available tasks or shutdown signal
                        {
                            std::unique_lock<std::mutex> lock(mQueueMutex);
                            mCondition.wait(lock, [this]() { return mStop || !mTasks.empty(); });

                            if (mStop && mTasks.empty())
                                return; // Exit thread

                            task = std::move(mTasks.front());
                            mTasks.pop();
                        }

                        try
                        {
                            task(); // Execute task
                        }
                        catch (const std::exception& e)
                        {
                            std::cerr << "[ThreadPool] Exception in task: " << e.what() << std::endl;
                        }
                        catch (...)
                        {
                            std::cerr << "[ThreadPool] Unknown exception in task." << std::endl;
                        }
                    }
                });
        }
    }

    // -------------------------------------------------------------------------
    // Enqueue: Adds a task to the queue
    // -------------------------------------------------------------------------
    void ThreadPool::Enqueue(std::function<void()> task)
    {
        {
            std::unique_lock<std::mutex> lock(mQueueMutex);
            if (mStop)
                throw std::runtime_error("ThreadPool is stopped. Cannot enqueue new tasks.");
            mTasks.emplace(std::move(task));
        }
        mCondition.notify_one();
    }

    // -------------------------------------------------------------------------
    // Shutdown: Gracefully stop all threads
    // -------------------------------------------------------------------------
    void ThreadPool::Shutdown()
    {
        {
            std::unique_lock<std::mutex> lock(mQueueMutex);
            mStop = true;
        }

        mCondition.notify_all();

        for (auto& worker : mWorkers)
        {
            if (worker.joinable())
                worker.join();
        }

        mWorkers.clear();
    }

    // -------------------------------------------------------------------------
    // Destructor: Ensures cleanup if not manually shut down
    // -------------------------------------------------------------------------
    ThreadPool::~ThreadPool()
    {
        Shutdown();
    }
}
