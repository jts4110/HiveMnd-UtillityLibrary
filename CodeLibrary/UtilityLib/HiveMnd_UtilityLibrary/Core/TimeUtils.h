#pragma once
#include <string>
#include <chrono>
#include <ctime>

/*
============================================================
File: TimeUtils.h
Purpose: Provides time-related helper functions for the
HiveMnd Utility Library Core module.
============================================================
*/

namespace HiveMnd
{
    namespace Core
    {
        /*
        ============================================================
        Class: TimeUtils
        Description:
            - Contains static methods for time management, formatting,
              and elapsed-time calculations.
            - Used throughout HiveMnd for timestamps and profiling.
        ============================================================
        */
        class TimeUtils
        {
        public:
            /*
            ============================================================
            Function: GetCurrentTimeString
            Returns: std::string
            Description:
                Gets the current system time and formats it as
                HH:MM:SS (24-hour format). Often used for logs.
            ============================================================
            */
            static std::string GetCurrentTimeString();

            /*
            ============================================================
            Function: GetDateString
            Returns: std::string
            Description:
                Gets the current date formatted as YYYY-MM-DD.
                Useful for reports, filenames, or logging.
            ============================================================
            */
            static std::string GetDateString();

            /*
            ============================================================
            Function: GetTimestamp
            Returns: std::string
            Description:
                Returns a compact string containing both date and
                time in the format YYYYMMDD_HHMMSS.
                Great for unique filenames or log entries.
            ============================================================
            */
            static std::string GetTimestamp();

            /*
            ============================================================
            Function: GetElapsedMilliseconds
            Parameters: std::chrono::steady_clock::time_point start
            Returns: long long
            Description:
                Calculates how many milliseconds have passed since
                a given start time. Useful for profiling execution
                time of operations or loops.
            ============================================================
            */
            static long long GetElapsedMilliseconds(std::chrono::steady_clock::time_point start);
        };
    }
}

