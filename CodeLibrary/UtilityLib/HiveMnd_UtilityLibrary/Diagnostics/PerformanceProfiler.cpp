#include "PerformanceProfiler.h"
#include <iomanip>
#include <sstream>

using namespace HiveMnd::Core;

namespace HiveMnd::Diagnostics
{
    void PerformanceProfiler::StartFrame()
    {
        std::lock_guard<std::mutex> lock(sMutex);
        sLastFrameTime = Clock::now();
    }

    void PerformanceProfiler::EndFrame()
    {
        std::lock_guard<std::mutex> lock(sMutex);
        auto now = Clock::now();
        sDeltaTime = std::chrono::duration<double>(now - sLastFrameTime).count();
        sFrameCount++;
        sAverageFrameTime = ((sAverageFrameTime * (sFrameCount - 1)) + sDeltaTime) / sFrameCount;
        sLastFrameTime = now;
    }

    double PerformanceProfiler::GetDeltaTime()
    {
        std::lock_guard<std::mutex> lock(sMutex);
        return sDeltaTime;
    }

    double PerformanceProfiler::GetAverageFPS()
    {
        std::lock_guard<std::mutex> lock(sMutex);
        return sAverageFrameTime > 0.0 ? 1.0 / sAverageFrameTime : 0.0;
    }

    void PerformanceProfiler::BeginSection(const std::string& sectionName)
    {
        std::lock_guard<std::mutex> lock(sMutex);
        sSectionStart[sectionName] = Clock::now();
    }

    void PerformanceProfiler::EndSection(const std::string& sectionName)
    {
        std::lock_guard<std::mutex> lock(sMutex);
        auto it = sSectionStart.find(sectionName);
        if (it == sSectionStart.end()) return;

        auto endTime = Clock::now();
        double ms = std::chrono::duration<double, std::milli>(endTime - it->second).count();
        sSectionDurations[sectionName] = ms;
        sSectionStart.erase(it);
    }

    void PerformanceProfiler::Report()
    {
        std::lock_guard<std::mutex> lock(sMutex);

        LogUtils::Info("Performance Profiler Report:");
        for (const auto& [name, ms] : sSectionDurations)
        {
            std::ostringstream ss;
            ss << " - " << name << ": " << std::fixed << std::setprecision(3) << ms << " ms";
            LogUtils::Debug(ss.str());
        }

        double fps = GetAverageFPS();
        std::ostringstream frameStats;
        frameStats << "Average FPS: " << std::fixed << std::setprecision(2) << fps
            << " | Avg Frame Time: " << (sAverageFrameTime * 1000.0) << " ms";
        LogUtils::Info(frameStats.str());
    }
}
