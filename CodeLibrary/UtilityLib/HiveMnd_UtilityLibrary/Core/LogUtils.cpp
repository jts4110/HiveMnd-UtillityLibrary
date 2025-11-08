#include "LogUtils.h"
#include "Core/TimeUtils.h"
#include <iomanip>

// Initialize static variables
bool LogUtils::sShowTimestamp = true;
bool LogUtils::sEnableDebug = true;
std::ofstream LogUtils::sLogFile;

// ============================================================================
// Sets the file path for log output
// ============================================================================
void LogUtils::SetLogFile(const std::string& filePath)
{
    if (sLogFile.is_open())
        sLogFile.close();

    sLogFile.open(filePath, std::ios::out | std::ios::app);
    if (!sLogFile)
    {
        std::cerr << "[LogUtils] Failed to open log file: " << filePath << std::endl;
    }
}

// ============================================================================
// Converts log level enum to string
// ============================================================================
std::string LogUtils::LevelToString(LogLevel level)
{
    switch (level)
    {
    case LogLevel::INFO: return "INFO";
    case LogLevel::WARNING: return "WARNING";
    case LogLevel::ERROR: return "ERROR";
    case LogLevel::DEBUG: return "DEBUG";
    default: return "UNKNOWN";
    }
}

// ============================================================================
// Logs a message with a given level and optional timestamp
// ============================================================================
void LogUtils::Log(const std::string& message, LogLevel level)
{
    if (level == LogLevel::DEBUG && !sEnableDebug)
        return; // Skip debug logs if disabled

    std::string timeStr = sShowTimestamp ? "[" + TimeUtils::GetTimestamp() + "] " : "";
    std::string levelStr = "[" + LevelToString(level) + "] ";

    std::string formatted = timeStr + levelStr + message;

    // Print to console
    std::cout << formatted << std::endl;

    // Write to file if one is open
    if (sLogFile.is_open())
    {
        sLogFile << formatted << std::endl;
    }
}

// ============================================================================
// Convenience wrappers for specific log types
// ============================================================================
void LogUtils::Info(const std::string& message)
{
    Log(message, LogLevel::INFO);
}

void LogUtils::Warn(const std::string& message)
{
    Log(message, LogLevel::WARNING);
}

void LogUtils::Error(const std::string& message)
{
    Log(message, LogLevel::ERROR);
}

void LogUtils::Debug(const std::string& message)
{
    Log(message, LogLevel::DEBUG);
}
