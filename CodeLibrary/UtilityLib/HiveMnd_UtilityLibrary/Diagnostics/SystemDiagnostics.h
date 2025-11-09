#pragma once
#include <string>
#include <vector>
#include <mutex>
#include "../Core/LogUtils.h"

#ifdef _WIN32
#include <windows.h>
#include <sysinfoapi.h>
#include <VersionHelpers.h>
#include <dxgi.h>
#pragma comment(lib, "dxgi.lib")
#endif

//
// HiveMnd :: Diagnostics :: SystemDiagnostics
// ---------------------------------------------------------
// Purpose : Retrieves system-level information like CPU name,
//           core count, total RAM, GPU adapter names, and OS details.
//
// Features:
//  • Works on Windows (full info) with safe fallbacks for other OSes
//  • Provides static GetSystemSummary() and LogSummary()
//  • Thread-safe and lightweight
// ---------------------------------------------------------
namespace HiveMnd::Diagnostics
{
    struct SystemInfoSnapshot
    {
        std::string cpuName;
        unsigned int coreCount = 0;
        unsigned long long totalRAM = 0;
        std::vector<std::string> gpus;
        std::string osVersion;

        std::string ToString() const;
    };

    class SystemDiagnostics
    {
    private:
        static inline std::mutex sMutex;

#ifdef _WIN32
        static std::string GetCPUName();
        static unsigned long long GetTotalRAM();
        static std::vector<std::string> GetGPUList();
        static std::string GetWindowsVersion();
#endif

    public:
        // ---------------------------------------------------------------------
        // Function: Collect
        // Purpose : Retrieves and returns a SystemInfoSnapshot with system data.
        // ---------------------------------------------------------------------
        static SystemInfoSnapshot Collect();

        // ---------------------------------------------------------------------
        // Function: LogSummary
        // Purpose : Logs a formatted report of the system information.
        // ---------------------------------------------------------------------
        static void LogSummary();
    };
}

