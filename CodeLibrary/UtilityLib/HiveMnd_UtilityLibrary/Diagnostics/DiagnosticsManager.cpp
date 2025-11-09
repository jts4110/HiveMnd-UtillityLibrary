#include "DiagnosticsManager.h"
#include <iomanip>
#include <sstream>

using namespace HiveMnd::Core;

namespace HiveMnd::Diagnostics
{
    void DiagnosticsManager::Initialize()
    {
        std::lock_guard<std::mutex> lock(sMutex);
        if (sInitialized)
        {
            LogUtils::Warning("DiagnosticsManager already initialized.");
            return;
        }

        sStartTime = std::chrono::steady_clock::now();
        sInitialized = true;

        LogUtils::Info("=== HiveMnd Diagnostics System Initialized ===");
        SystemDiagnostics::LogSummary();
        MemoryMonitor::ReportCurrent();
        LogUtils::Info("------------------------------------------------");
    }

    void DiagnosticsManager::RunFullDiagnostics()
    {
        std::lock_guard<std::mutex> lock(sMutex);
        if (!sInitialized)
        {
            LogUtils::Error("DiagnosticsManager not initialized! Call Initialize() first.");
            return;
        }

        LogUtils::Info("=== Running Full System Diagnostics ===");

        // Step 1: Report System Snapshot
        SystemDiagnostics::LogSummary();

        // Step 2: Report Current Memory
        MemoryMonitor::ReportCurrent();

        // Step 3: Measure short performance sample
        PerformanceProfiler perf("FullDiagnostics");
        perf.BeginSample("Diagnostics Sample");
        for (volatile int i = 0; i < 10'000'000; ++i); // dummy load
        perf.EndSample("Diagnostics Sample");
        perf.LogSummary();

        // Step 4: Report Uptime
        ReportUptime();

        LogUtils::Success("=== Diagnostics Completed Successfully ===");
    }

    void DiagnosticsManager::ReportUptime()
    {
        if (!sInitialized) return;

        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - sStartTime);
        int hours = static_cast<int>(duration.count() / 3600);
        int minutes = static_cast<int>((duration.count() % 3600) / 60);
        int seconds = static_cast<int>(duration.count() % 60);

        std::ostringstream ss;
        ss << "Uptime: " << std::setw(2) << std::setfill('0') << hours << ":"
            << std::setw(2) << minutes << ":" << std::setw(2) << seconds;

        LogUtils::Info(ss.str());
    }

    void DiagnosticsManager::Shutdown()
    {
        std::lock_guard<std::mutex> lock(sMutex);
        if (!sInitialized)
        {
            LogUtils::Warning("DiagnosticsManager was not initialized; skipping shutdown.");
            return;
        }

        LogUtils::Info("Shutting down HiveMnd Diagnostics...");
        ReportUptime();
        MemoryMonitor::ReportCurrent();
        LogUtils::Info("=== Diagnostics System Shutdown Complete ===");

        sInitialized = false;
    }
}
