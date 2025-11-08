#include "TimeUtils.h"

namespace TimeUtils
{
    // GetTimestamp
    // --------------------------------------------
    // Builds a formatted string for the current
    // date and time using the system clock.
    // Example output: "2025-11-08 12:34:56"
    // --------------------------------------------
    std::string GetTimestamp()
    {
        auto now = std::chrono::system_clock::now();
        std::time_t nowTime = std::chrono::system_clock::to_time_t(now);

        std::tm localTime{};
        localtime_s(&localTime, &nowTime);

        std::ostringstream oss;
        oss << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }

    // GetDate
    // --------------------------------------------
    // Returns only the current date.
    // Example output: "2025-11-08"
    // --------------------------------------------
    std::string GetDate()
    {
        auto now = std::chrono::system_clock::now();
        std::time_t nowTime = std::chrono::system_clock::to_time_t(now);

        std::tm localTime{};
        localtime_s(&localTime, &nowTime);

        std::ostringstream oss;
        oss << std::put_time(&localTime, "%Y-%m-%d");
        return oss.str();
    }

    // GetTime
    // --------------------------------------------
    // Returns only the current time.
    // Example output: "12:34:56"
    // --------------------------------------------
    std::string GetTime()
    {
        auto now = std::chrono::system_clock::now();
        std::time_t nowTime = std::chrono::system_clock::to_time_t(now);

        std::tm localTime{};
        localtime_s(&localTime, &nowTime);

        std::ostringstream oss;
        oss << std::put_time(&localTime, "%H:%M:%S");
        return oss.str();
    }

    // GetMilliseconds
    // --------------------------------------------
    // Returns the number of milliseconds since the
    // Unix epoch (January 1, 1970). Useful for
    // precise timing or performance measurement.
    // --------------------------------------------
    long long GetMilliseconds()
    {
        auto now = std::chrono::system_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()).count();
        return ms;
    }
}

