#include "SystemUtils.h"
#include <filesystem>
#include <thread>
#include <cstdlib>
#include <iostream>

#ifdef _WIN32
#include <Windows.h>
#include <Lmcons.h>
#endif

namespace SystemUtils
{
    std::string GetOSName()
    {
#ifdef _WIN32
        return "Windows";
#elif __APPLE__
        return "macOS";
#elif __linux__
        return "Linux";
#else
        return "Unknown OS";
#endif
    }

    std::string GetUserName()
    {
#ifdef _WIN32
        // Use TCHAR for compatibility with both Unicode and Multi-Byte builds
        TCHAR username[UNLEN + 1];
        DWORD size = UNLEN + 1;
        ::GetUserName(username, &size); // Explicitly call the global Windows API function
        return std::string(username);
#else
        const char* user = getenv("USER");
        return user ? std::string(user) : "Unknown";
#endif
    }


    std::string GetWorkingDirectory()
    {
        return std::filesystem::current_path().string();
    }

    bool FileExists(const std::string& path)
    {
        return std::filesystem::is_regular_file(path);
    }

    bool DirectoryExists(const std::string& path)
    {
        return std::filesystem::is_directory(path);
    }

    void ClearConsole()
    {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
    }

    void Pause()
    {
        std::cout << "Press Enter to continue...";
        std::cin.get();
    }

    unsigned int GetCPUCount()
    {
        return std::thread::hardware_concurrency();
    }
}
