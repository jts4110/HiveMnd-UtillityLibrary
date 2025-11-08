#include "TaskScheduler.h"
#include <iostream>

namespace HiveMnd::Core
{
    // -------------------------------------------------------------------------
    // Constructor: Starts scheduler thread and initializes thread pool
    // -------------------------------------------------------------------------
    TaskScheduler::TaskScheduler(size_t poolSize)
        : mRunning(true), mThreadPool(poolSize)
    {
        mSchedulerThread = std::thread(&TaskScheduler::SchedulerLoop, this);
    }

    // -------------------------------------------------------------------------
    // Destructor: Ensures clean shutdown
    // -------------------------------------------------------------------------
    TaskScheduler::~TaskScheduler()
    {
        Shutdown();
    }

    // -------------------------------------------------------------------------
    // Schedule: Adds a single delayed task
    // -------------------------------------------------------------------------
    void TaskScheduler::Schedule(const std::string& id, const std::function<void()>& func, unsigned int delayMs)
    {
        std::unique_lock<std::mutex> lock(mMutex);
        ScheduledTask task;
        task.func = func;
        task.nextRun = std::chrono::steady_clock::now() + std::chrono::milliseconds(delayMs);
        task.interval = std::chrono::milliseconds(0);
        task.repeat = false;
        mTasks[id] = std::move(task);
        mCondition.notify_all();
    }

    // -------------------------------------------------------------------------
    // ScheduleRepeating: Adds a task that repeats every intervalMs milliseconds
    // -------------------------------------------------------------------------
    void TaskScheduler::ScheduleRepeating(const std::string& id, const std::function<void()>& func, unsigned int intervalMs)
    {
        std::unique_lock<std::mutex> lock(mMutex);
        ScheduledTask task;
        task.func = func;
        task.nextRun = std::chrono::steady_clock::now() + std::chrono::milliseconds(intervalMs);
        task.interval = std::chrono::milliseconds(intervalMs);
        task.repeat = true;
        mTasks[id] = std::move(task);
        mCondition.notify_all();
    }

    // -------------------------------------------------------------------------
    // Cancel: Removes a task from the scheduler
    // -------------------------------------------------------------------------
    void TaskScheduler::Cancel(const std::string& id)
    {
        std::unique_lock<std::mutex> lock(mMutex);
        mTasks.erase(id);
    }

    // -------------------------------------------------------------------------
    // SchedulerLoop: Background thread that triggers tasks at proper times
    // -------------------------------------------------------------------------
    void TaskScheduler::SchedulerLoop()
    {
        while (mRunning)
        {
            std::unique_lock<std::mutex> lock(mMutex);

            if (mTasks.empty())
            {
                mCondition.wait(lock, [this]() { return !mRunning || !mTasks.empty(); });
                continue;
            }

            auto now = std::chrono::steady_clock::now();
            auto nextTaskIt = std::min_element(mTasks.begin(), mTasks.end(),
                [](const auto& a, const auto& b)
                {
                    return a.second.nextRun < b.second.nextRun;
                });

            if (nextTaskIt == mTasks.end())
            {
                mCondition.wait_for(lock, std::chrono::milliseconds(100));
                continue;
            }

            auto& [id, task] = *nextTaskIt;
            if (now >= task.nextRun)
            {
                mThreadPool.Enqueue(task.func);

                if (task.repeat)
                    task.nextRun = now + task.interval;
                else
                    mTasks.erase(id);
            }
            else
            {
                mCondition.wait_until(lock, task.nextRun);
            }
        }
    }

    // -------------------------------------------------------------------------
    // Shutdown: Stops scheduler and worker threads
    // -------------------------------------------------------------------------
    void TaskScheduler::Shutdown()
    {
        {
            std::unique_lock<std::mutex> lock(mMutex);
            mRunning = false;
        }

        mCondition.notify_all();

        if (mSchedulerThread.joinable())
            mSchedulerThread.join();

        mThreadPool.Shutdown();
    }
}
