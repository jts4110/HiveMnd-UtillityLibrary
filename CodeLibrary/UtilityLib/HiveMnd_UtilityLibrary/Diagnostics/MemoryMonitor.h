#pragma once
#include <string>
#include <mutex>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "../Core/LogUtils.h"

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#endif

//
// HiveMnd :: Diagnostics :: MemoryMonitor
// ---------------------------------------------------------
// Purpose : Tracks application memory usage and allows snapshot
//           comparisons to detect leaks or excessive allocations.
//
// Features:
//  • Gets current and peak memory usage
//  • Supports before/after delta snapshot comparisons
//  • Thread-safe with logging via LogUtils
// ---------------------------------------------------------
namespace HiveMnd::Diagnostics
{
    struct MemorySnapshot
    {
        size_t workingSet = 0;     // Current memory usage in bytes
        size_t peakWorkingSet = 0; // Peak memory usage in bytes
        size_t privateUsage = 0;   // Private bytes (if available)

        std::string ToString() const;
    };

    class MemoryMonitor
    {
    private:
        static inline std::mutex sMutex;
        static MemorySnapshot CaptureSnapshot();

    public:
        // ---------------------------------------------------------------------
        // Function: GetCurrentUsage
        // Purpose : Returns a MemorySnapshot of current usage.
        // ---------------------------------------------------------------------
        static MemorySnapshot GetCurrentUsage();

        // ---------------------------------------------------------------------
        // Function: ReportCurrent
        // Purpose : Logs current memory stats to console/log.
        // ---------------------------------------------------------------------
        static void ReportCurrent();

        // ---------------------------------------------------------------------
        // Function: CompareSnapshots
        // Purpose : Prints the difference between two snapshots.
        // ---------------------------------------------------------------------
        static void CompareSnapshots(const MemorySnapshot& before,
            const MemorySnapshot& after);
    };
}

