#pragma once
#include <string>

//
// HiveMnd::Core - System Utilities
// -----------------------------------------------------------
// Provides low-level helper functions for system information,
// environment queries, and common console operations.
// -----------------------------------------------------------
// Example:
//     std::string user = SystemUtils::GetUserName();
//     SystemUtils::ClearConsole();
// -----------------------------------------------------------
//

namespace SystemUtils
{
    // Returns the current operating system name
    std::string GetOSName();

    // Returns the name of the current logged-in user
    std::string GetUserName();

    // Returns the current working directory path
    std::string GetWorkingDirectory();

    // Checks whether a given file exists
    bool FileExists(const std::string& path);

    // Checks whether a directory exists
    bool DirectoryExists(const std::string& path);

    // Clears the console window
    void ClearConsole();

    // Pauses execution until user input
    void Pause();

    // Returns the number of CPU cores available
    unsigned int GetCPUCount();
}

