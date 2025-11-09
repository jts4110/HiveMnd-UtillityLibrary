#pragma once
#include <string>
#include <unordered_map>
#include <chrono>
#include <mutex>
#include "../Core/LogUtils.h"

//
// HiveMnd :: Diagnostics :: PerformanceProfiler
// ---------------------------------------------------------
// Purpose : Provides high-precision performance timing tools
//           for benchmarking code segments.
//
// Features:
//  • Start and stop named samples
//  • Calculate elapsed times in ms
//  • Thread-safe multi-sample support
//  • Integrated with LogUtils for formatted output
// ---------------------------------------------------------
namespace HiveMnd::Diagnostics
{
    class PerformanceProfiler
    {
    private:
        std::string mName;
        std::unordered_map<std::string, std::chrono::high_resolution_clock::time_point> mStartTimes;
        std::unordered_map<std::string, double> mDurations;
        std::mutex mMutex;

    public:
        // Constructor sets profile label
        explicit PerformanceProfiler(const std::string& name = "UnnamedProfiler");

        // Begin timing a sample
        void BeginSample(const std::string& label);

        // End timing a sample and record duration
        void EndSample(const std::string& label);

        // Retrieve elapsed time for a sample
        double GetElapsed(const std::string& label) const;

        // Logs all recorded samples and averages
        void LogSummary() const;

        // Clears stored sample data
        void Reset();
    };
}
