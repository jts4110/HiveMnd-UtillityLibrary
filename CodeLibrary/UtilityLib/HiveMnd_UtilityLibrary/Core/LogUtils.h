#pragma once
#include <string>
#include <fstream>
#include <mutex>

// -----------------------------------------------------------------------------
//  Utility: HiveMnd::Core::LogUtils
//  Purpose : Provides leveled logging with timestamping, colorized console
//            output, and optional file writing for HiveMnd projects.
//  Notes   : Thread-safe and compatible with TimeUtils + DebugUtils.
// -----------------------------------------------------------------------------
namespace HiveMnd::Core
{
    enum class LogLevel
    {
        Info,
        Warning,
        Error,
        Debug
    };

    class LogUtils
    {
    private:
        static inline bool sEnableColors = true;
        static inline bool sShowTimestamp = true;
        static inline bool sLogToFile = false;
        static inline std::string sLogFilePath = "HiveMnd_Log.txt";
        static inline std::ofstream sLogFile;
        static inline std::mutex sLogMutex;

        // Internal helper for writing formatted output
        static void Write(const std::string& prefix,
            const std::string& message,
            const std::string& colorCode);

    public:
        // ---------------------------------------------------------------------
        //  Function: Initialize
        //  Purpose : Prepares log file if file logging is enabled.
        // ---------------------------------------------------------------------
        static void Initialize(bool logToFile = false,
            const std::string& filePath = "HiveMnd_Log.txt");

        // ---------------------------------------------------------------------
        //  Function: Shutdown
        //  Purpose : Closes any open log file.
        // ---------------------------------------------------------------------
        static void Shutdown();

        // ---------------------------------------------------------------------
        //  Function: SetShowTimestamp
        //  Purpose : Enable or disable timestamp display.
        // ---------------------------------------------------------------------
        static void SetShowTimestamp(bool enable);

        // ---------------------------------------------------------------------
        //  Function: SetEnableColors
        //  Purpose : Enable or disable colored console output.
        // ---------------------------------------------------------------------
        static void SetEnableColors(bool enable);

        // ---------------------------------------------------------------------
        //  Function: Info / Warning / Error / Debug
        //  Purpose : Main logging functions with level categorization.
        // ---------------------------------------------------------------------
        static void Info(const std::string& message);
        static void Warning(const std::string& message);
        static void Error(const std::string& message);
        static void Debug(const std::string& message);
    };
}


