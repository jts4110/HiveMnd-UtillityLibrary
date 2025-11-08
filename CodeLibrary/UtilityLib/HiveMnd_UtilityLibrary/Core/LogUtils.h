#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include "TimeUtils.h" 

// ============================================================================
// LOG UTILS - Core logging system for HiveMnd Utility Library
// ----------------------------------------------------------------------------
// Provides simple static logging functions for console and file output.
// Logs can include timestamps and log levels (INFO, WARN, ERROR, DEBUG).
// ============================================================================

class LogUtils
{
public:
    // Enumeration for log levels
    enum class LogLevel
    {
        INFO,
        WARNING,
        ERROR,
        DEBUG
    };

    // Enables or disables timestamp printing
    static bool sShowTimestamp;

    // Enables or disables debug log printing
    static bool sEnableDebug;

    // Sets a log file path for file output (optional)
    static void SetLogFile(const std::string& filePath);

    // Main log function - prints message with level
    static void Log(const std::string& message, LogLevel level = LogLevel::INFO);

    // Specialized shortcuts
    static void Info(const std::string& message);
    static void Warn(const std::string& message);
    static void Error(const std::string& message);
    static void Debug(const std::string& message);

private:
    // Converts log level enum to a readable string
    static std::string LevelToString(LogLevel level);

    // Optional file stream for writing logs to disk
    static std::ofstream sLogFile;
};

