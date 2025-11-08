#pragma once
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <vector>
#include <atomic>
#include <map>
#include <string>
#include "ThreadPool.h"

// -----------------------------------------------------------------------------
// HiveMnd Utility Library : TaskScheduler
// Purpose: Provides delayed, repeating, and timed task scheduling.
// Notes  : Built on top of ThreadPool for efficient async execution.
// -----------------------------------------------------------------------------
namespace HiveMnd::Core
{
    class TaskScheduler
    {
    private:
        struct ScheduledTask
        {
            std::function<void()> func = nullptr;                      // Task function pointer
            std::chrono::steady_clock::time_point nextRun{};           // Next execution time (default = now)
            std::chrono::milliseconds interval{ 0 };                     // Delay or repeat interval
            bool repeat = false;                                       // Whether the task repeats
        };


        std::mutex mMutex;                                   // Synchronization lock
        std::condition_variable mCondition;                  // Signals scheduler thread
        std::atomic<bool> mRunning;                          // Controls scheduler loop
        std::thread mSchedulerThread;                        // Dedicated scheduler thread
        ThreadPool mThreadPool;                              // Worker pool for executing tasks
        std::map<std::string, ScheduledTask> mTasks;         // Task registry

        // Internal loop that manages timing and task dispatch
        void SchedulerLoop();

    public:
        // ---------------------------------------------------------------------
        //  Constructor / Destructor
        // ---------------------------------------------------------------------
        TaskScheduler(size_t poolSize = std::thread::hardware_concurrency());
        ~TaskScheduler();

        // ---------------------------------------------------------------------
        //  Function: Schedule
        //  Purpose : Schedules a one-time task after a delay.
        // ---------------------------------------------------------------------
        void Schedule(const std::string& id, const std::function<void()>& func, unsigned int delayMs);

        // ---------------------------------------------------------------------
        //  Function: ScheduleRepeating
        //  Purpose : Schedules a task that repeats at the given interval.
        // ---------------------------------------------------------------------
        void ScheduleRepeating(const std::string& id, const std::function<void()>& func, unsigned int intervalMs);

        // ---------------------------------------------------------------------
        //  Function: Cancel
        //  Purpose : Cancels a scheduled task by ID.
        // ---------------------------------------------------------------------
        void Cancel(const std::string& id);

        // ---------------------------------------------------------------------
        //  Function: Shutdown
        //  Purpose : Gracefully stops scheduler and worker threads.
        // ---------------------------------------------------------------------
        void Shutdown();
    };
}

