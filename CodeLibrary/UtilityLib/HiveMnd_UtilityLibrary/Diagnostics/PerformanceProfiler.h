#pragma once
#include <chrono>
#include <string>
#include <unordered_map>
#include <mutex>
#include "../Core/LogUtils.h"
#include "../Core/TimeUtils.h"

//
// HiveMnd :: Diagnostics :: PerformanceProfiler
// ---------------------------------------------------------
// Purpose : Provides timing and performance measurement tools
//           for profiling code execution speed and stability.
//
// Features:
//  • Measure total runtime, delta time, and average FPS
//  • Mark custom sections to measure their duration
//  • Thread-safe (mutex protected)
//  • Uses high-precision steady_clock
// ---------------------------------------------------------
namespace HiveMnd::Diagnostics
{
    class PerformanceProfiler
    {
    private:
        using Clock = std::chrono::steady_clock;
        static inline std::unordered_map<std::string, Clock::time_point> sSectionStart;
        static inline std::unordered_map<std::string, double> sSectionDurations;
        static inline Clock::time_point sLastFrameTime;
        static inline double sDeltaTime;
        static inline double sAverageFrameTime;
        static inline size_t sFrameCount;
        static inline std::mutex sMutex;

    public:
        // ---------------------------------------------------------------------
        // Function: StartFrame
        // Purpose : Marks the beginning of a new frame or update cycle.
        // ---------------------------------------------------------------------
        static void StartFrame();

        // ---------------------------------------------------------------------
        // Function: EndFrame
        // Purpose : Ends the current frame and updates delta time and averages.
        // ---------------------------------------------------------------------
        static void EndFrame();

        // ---------------------------------------------------------------------
        // Function: GetDeltaTime
        // Purpose : Returns time (in seconds) between the last two frames.
        // ---------------------------------------------------------------------
        static double GetDeltaTime();

        // ---------------------------------------------------------------------
        // Function: GetAverageFPS
        // Purpose : Returns the rolling average FPS across frames.
        // ---------------------------------------------------------------------
        static double GetAverageFPS();

        // ---------------------------------------------------------------------
        // Function: BeginSection
        // Purpose : Starts timing a named section (e.g., "Physics", "AI").
        // ---------------------------------------------------------------------
        static void BeginSection(const std::string& sectionName);

        // ---------------------------------------------------------------------
        // Function: EndSection
        // Purpose : Ends a section and records its elapsed time in milliseconds.
        // ---------------------------------------------------------------------
        static void EndSection(const std::string& sectionName);

        // ---------------------------------------------------------------------
        // Function: Report
        // Purpose : Logs a summary of all tracked section durations.
        // ---------------------------------------------------------------------
        static void Report();
    };
}

