#include "TimeUtils.h"
#include <iomanip> // for std::put_time
#include <sstream> // for std::stringstream

/*
============================================================
File: TimeUtils.cpp
Implements the static methods declared in TimeUtils.h.
============================================================
*/

namespace HiveMnd
{
    namespace Core
    {
        // ============================================================
        // Returns the current local system time as HH:MM:SS
        // ============================================================
        std::string TimeUtils::GetCurrentTimeString()
        {
            auto now = std::chrono::system_clock::now();
            std::time_t timeNow = std::chrono::system_clock::to_time_t(now);

            std::tm localTime{};
#ifdef _WIN32
            localtime_s(&localTime, &timeNow);
#else
            localtime_r(&timeNow, &localTime);
#endif

            std::stringstream ss;
            ss << std::put_time(&localTime, "%H:%M:%S");
            return ss.str();
        }

        // ============================================================
        // Returns the current date as YYYY-MM-DD
        // ============================================================
        std::string TimeUtils::GetDateString()
        {
            auto now = std::chrono::system_clock::now();
            std::time_t timeNow = std::chrono::system_clock::to_time_t(now);

            std::tm localTime{};
#ifdef _WIN32
            localtime_s(&localTime, &timeNow);
#else
            localtime_r(&timeNow, &localTime);
#endif

            std::stringstream ss;
            ss << std::put_time(&localTime, "%Y-%m-%d");
            return ss.str();
        }

        // ============================================================
        // Combines date and time into a compact timestamp format
        // Example: 20251108_114600
        // ============================================================
        std::string TimeUtils::GetTimestamp()
        {
            auto now = std::chrono::system_clock::now();
            std::time_t timeNow = std::chrono::system_clock::to_time_t(now);

            std::tm localTime{};
#ifdef _WIN32
            localtime_s(&localTime, &timeNow);
#else
            localtime_r(&timeNow, &localTime);
#endif

            std::stringstream ss;
            ss << std::put_time(&localTime, "%Y%m%d_%H%M%S");
            return ss.str();
        }

        // ============================================================
        // Returns how many milliseconds have elapsed since 'start'.
        // This helps measure function or loop execution time.
        // ============================================================
        long long TimeUtils::GetElapsedMilliseconds(std::chrono::steady_clock::time_point start)
        {
            auto now = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
            return duration.count();
        }
    }
}
