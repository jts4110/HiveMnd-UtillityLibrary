#pragma once
#include <string>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

//
// TimeUtils
// --------------------------------------------
// Provides helper functions for working with
// system time, including timestamps, dates,
// and time strings with optional millisecond
// precision.
//
// Example usage:
//     std::string ts = TimeUtils::GetTimestamp();
//     std::string ms = TimeUtils::GetMilliseconds();
// --------------------------------------------
//

namespace TimeUtils
{
    // Returns a full timestamp in the format "YYYY-MM-DD HH:MM:SS"
    std::string GetTimestamp();

    // Returns only the date in the format "YYYY-MM-DD"
    std::string GetDate();

    // Returns only the time in the format "HH:MM:SS"
    std::string GetTime();

    // Returns the current time in milliseconds since epoch
    // Useful for measuring performance or generating unique IDs
    long long GetMilliseconds();
}


