#include "PerformanceProfiler.h"
#include <sstream>
#include <iomanip>

using namespace HiveMnd::Core;

namespace HiveMnd::Diagnostics
{
    PerformanceProfiler::PerformanceProfiler(const std::string& name)
        : mName(name)
    {
    }

    void PerformanceProfiler::BeginSample(const std::string& label)
    {
        std::lock_guard<std::mutex> lock(mMutex);
        mStartTimes[label] = std::chrono::high_resolution_clock::now();
    }

    void PerformanceProfiler::EndSample(const std::string& label)
    {
        std::lock_guard<std::mutex> lock(mMutex);
        auto it = mStartTimes.find(label);
        if (it != mStartTimes.end())
        {
            auto end = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration<double, std::milli>(end - it->second).count();
            mDurations[label] = elapsed;
            mStartTimes.erase(it);
        }
        else
        {
            LogUtils::Warning("Attempted to end unknown sample: " + label);
        }
    }

    double PerformanceProfiler::GetElapsed(const std::string& label) const
    {
        auto it = mDurations.find(label);
        if (it != mDurations.end())
            return it->second;
        return 0.0;
    }

    void PerformanceProfiler::LogSummary() const
    {
        std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(mMutex));
        if (mDurations.empty())
        {
            LogUtils::Info("[" + mName + "] No performance samples recorded.");
            return;
        }

        std::ostringstream ss;
        ss << "=== Performance Summary: " << mName << " ===\n";
        double total = 0.0;

        for (const auto& [label, time] : mDurations)
        {
            ss << "  • " << std::left << std::setw(25) << label
                << " : " << std::right << std::fixed << std::setprecision(3)
                << time << " ms\n";
            total += time;
        }

        ss << "-----------------------------------------\n";
        ss << "Total Time: " << total << " ms (" << mDurations.size() << " samples)";
        LogUtils::Info(ss.str());
    }

    void PerformanceProfiler::Reset()
    {
        std::lock_guard<std::mutex> lock(mMutex);
        mStartTimes.clear();
        mDurations.clear();
    }
}
