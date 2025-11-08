#include "Core/LogUtils.h"
#include "Core/TimeUtils.h"
#include "Core/DebugUtils.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>

using namespace HiveMnd::Core;

// -----------------------------------------------------------------------------
//  Function: Initialize
//  Purpose : Opens a file for logging if enabled.
// -----------------------------------------------------------------------------
void LogUtils::Initialize(bool logToFile, const std::string& filePath)
{
    sLogToFile = logToFile;
    sLogFilePath = filePath;

    if (sLogToFile)
    {
        sLogFile.open(sLogFilePath, std::ios::out | std::ios::app);
        if (!sLogFile.is_open())
        {
            DebugUtils::Trace("[LogUtils] Failed to open log file: " + sLogFilePath);
        }
    }
}

// -----------------------------------------------------------------------------
//  Function: Shutdown
//  Purpose : Closes the open log file safely.
// -----------------------------------------------------------------------------
void LogUtils::Shutdown()
{
    if (sLogFile.is_open())
        sLogFile.close();
}

// -----------------------------------------------------------------------------
//  Function: SetShowTimestamp
// -----------------------------------------------------------------------------
void LogUtils::SetShowTimestamp(bool enable)
{
    sShowTimestamp = enable;
}

// -----------------------------------------------------------------------------
//  Function: SetEnableColors
// -----------------------------------------------------------------------------
void LogUtils::SetEnableColors(bool enable)
{
    sEnableColors = enable;
}

// -----------------------------------------------------------------------------
//  Function: Write
//  Purpose : Handles synchronized console + file output.
// -----------------------------------------------------------------------------
void LogUtils::Write(const std::string& prefix,
    const std::string& message,
    const std::string& colorCode)
{
    std::lock_guard<std::mutex> lock(sLogMutex);

    std::ostringstream oss;

    if (sShowTimestamp)
        oss << "[" << TimeUtils::GetTimestamp() << "] ";

    oss << prefix << ": " << message;

    std::string finalMsg = oss.str();

    // Console output
    if (sEnableColors)
        std::cout << colorCode << finalMsg << "\033[0m" << std::endl;
    else
        std::cout << finalMsg << std::endl;

    // File output
    if (sLogToFile && sLogFile.is_open())
        sLogFile << finalMsg << std::endl;
}

// -----------------------------------------------------------------------------
//  Function: Info
// -----------------------------------------------------------------------------
void LogUtils::Info(const std::string& message)
{
    Write("[INFO]", message, "\033[36m"); // Cyan
}

// -----------------------------------------------------------------------------
//  Function: Warning
// -----------------------------------------------------------------------------
void LogUtils::Warning(const std::string& message)
{
    Write("[WARN]", message, "\033[33m"); // Yellow
}

// -----------------------------------------------------------------------------
//  Function: Error
// -----------------------------------------------------------------------------
void LogUtils::Error(const std::string& message)
{
    Write("[ERROR]", message, "\033[31m"); // Red
}

// -----------------------------------------------------------------------------
//  Function: Debug
// -----------------------------------------------------------------------------
void LogUtils::Debug(const std::string& message)
{
#ifdef _DEBUG
    Write("[DEBUG]", message, "\033[35m"); // Magenta
#endif
}
