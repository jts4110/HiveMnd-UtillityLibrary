#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>
#include <functional>
#include <vector>
#include <string>
#include <iostream>

// -----------------------------------------------------------------------------
// HiveMnd Utility Library : ThreadUtils
// Purpose: Simple helpers for thread operations, sleeping, and async launching.
// Notes  : Wraps standard C++ <thread> functionality in safe, consistent methods.
// -----------------------------------------------------------------------------
namespace HiveMnd::Core
{
    class ThreadUtils
    {
    public:
        // ---------------------------------------------------------------------
        //  Function: Sleep
        //  Purpose : Suspends execution for the specified milliseconds.
        // ---------------------------------------------------------------------
        static void Sleep(unsigned int milliseconds)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
        }

        // ---------------------------------------------------------------------
        //  Function: GetCurrentThreadID
        //  Purpose : Returns the numeric ID of the calling thread.
        // ---------------------------------------------------------------------
        static unsigned long GetCurrentThreadID()
        {
            std::hash<std::thread::id> hasher;
            return static_cast<unsigned long>(hasher(std::this_thread::get_id()));
        }

        // ---------------------------------------------------------------------
        //  Function: RunAsync
        //  Purpose : Runs a function asynchronously in a detached thread.
        // ---------------------------------------------------------------------
        static void RunAsync(const std::function<void()>& func)
        {
            std::thread(func).detach();
        }

        // ---------------------------------------------------------------------
        //  Function: RunBlocking
        //  Purpose : Runs a function synchronously and waits for it to complete.
        // ---------------------------------------------------------------------
        static void RunBlocking(const std::function<void()>& func)
        {
            std::thread t(func);
            t.join();
        }

        // ---------------------------------------------------------------------
        //  Function: WaitUntil
        //  Purpose : Blocks until a condition becomes true or timeout occurs.
        // ---------------------------------------------------------------------
        template<typename Predicate>
        static bool WaitUntil(Predicate condition, unsigned int timeoutMs = 5000)
        {
            auto start = std::chrono::steady_clock::now();
            while (!condition())
            {
                Sleep(10);
                if (std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::steady_clock::now() - start).count() >= timeoutMs)
                    return false;
            }
            return true;
        }
    };
}

