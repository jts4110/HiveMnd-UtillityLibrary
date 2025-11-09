#pragma once
#include <string>
#include <mutex>
#include <chrono>
#include "../Core/LogUtils.h"
#include "PerformanceProfiler.h"
#include "MemoryMonitor.h"
#include "SystemDiagnostics.h"

//
// HiveMnd :: Diagnostics :: DiagnosticsManager
// ---------------------------------------------------------
// Purpose : Centralized manager that coordinates all diagnostics
//           systems (PerformanceProfiler, MemoryMonitor,
//           SystemDiagnostics) to produce unified reports.
//
// Features:
//  • One-call "RunFullDiagnostics()" for combined system checks
//  • Tracks runtime duration and session start time
//  • Supports periodic reporting and threshold-based warnings
// ---------------------------------------------------------
namespace HiveMnd::Diagnostics
{
    class DiagnosticsManager
    {
    private:
        static inline std::mutex sMutex;
        static inline std::chrono::steady_clock::time_point sStartTime{};
        static inline bool sInitialized = false;

    public:
        // ---------------------------------------------------------------------
        // Function: Initialize
        // Purpose : Starts timers and logs environment details.
        // ---------------------------------------------------------------------
        static void Initialize();

        // ---------------------------------------------------------------------
        // Function: RunFullDiagnostics
        // Purpose : Runs system, memory, and performance diagnostics together.
        // ---------------------------------------------------------------------
        static void RunFullDiagnostics();

        // ---------------------------------------------------------------------
        // Function: ReportUptime
        // Purpose : Prints how long the current session has been running.
        // ---------------------------------------------------------------------
        static void ReportUptime();

        // ---------------------------------------------------------------------
        // Function: Shutdown
        // Purpose : Finalizes and logs cleanup confirmation.
        // ---------------------------------------------------------------------
        static void Shutdown();
    };
}

