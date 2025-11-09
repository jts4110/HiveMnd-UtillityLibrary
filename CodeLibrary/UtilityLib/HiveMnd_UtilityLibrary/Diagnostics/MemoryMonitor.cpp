#include "MemoryMonitor.h"

using namespace HiveMnd::Core;

namespace HiveMnd::Diagnostics
{
    std::string MemorySnapshot::ToString() const
    {
        std::ostringstream ss;
        ss << "Working Set: " << std::fixed << std::setprecision(2)
            << (workingSet / (1024.0 * 1024.0)) << " MB"
            << " | Peak: " << (peakWorkingSet / (1024.0 * 1024.0)) << " MB"
            << " | Private: " << (privateUsage / (1024.0 * 1024.0)) << " MB";
        return ss.str();
    }

    MemorySnapshot MemoryMonitor::CaptureSnapshot()
    {
        std::lock_guard<std::mutex> lock(sMutex);
        MemorySnapshot snap{};

#ifdef _WIN32
        PROCESS_MEMORY_COUNTERS_EX pmc{};
        if (GetProcessMemoryInfo(GetCurrentProcess(),
            reinterpret_cast<PPROCESS_MEMORY_COUNTERS>(&pmc),
            sizeof(pmc)))
        {
            snap.workingSet = pmc.WorkingSetSize;
            snap.peakWorkingSet = pmc.PeakWorkingSetSize;
            snap.privateUsage = pmc.PrivateUsage;
        }
#else
        // Cross-platform placeholder (Linux/macOS)
        snap.workingSet = 0;
        snap.peakWorkingSet = 0;
        snap.privateUsage = 0;
#endif

        return snap;
    }

    MemorySnapshot MemoryMonitor::GetCurrentUsage()
    {
        return CaptureSnapshot();
    }

    void MemoryMonitor::ReportCurrent()
    {
        MemorySnapshot snap = CaptureSnapshot();
        LogUtils::Info("Memory Usage: " + snap.ToString());
    }

    void MemoryMonitor::CompareSnapshots(const MemorySnapshot& before, const MemorySnapshot& after)
    {
        size_t deltaWorking = (after.workingSet > before.workingSet)
            ? after.workingSet - before.workingSet
            : 0;
        size_t deltaPeak = (after.peakWorkingSet > before.peakWorkingSet)
            ? after.peakWorkingSet - before.peakWorkingSet
            : 0;
        size_t deltaPrivate = (after.privateUsage > before.privateUsage)
            ? after.privateUsage - before.privateUsage
            : 0;

        std::ostringstream ss;
        ss << "Memory Change → "
            << "Working: +" << std::fixed << std::setprecision(2)
            << (deltaWorking / (1024.0 * 1024.0)) << " MB, "
            << "Peak: +" << (deltaPeak / (1024.0 * 1024.0)) << " MB, "
            << "Private: +" << (deltaPrivate / (1024.0 * 1024.0)) << " MB";

        if (deltaWorking > (10 * 1024 * 1024)) // >10 MB growth warning
            LogUtils::Warning(ss.str());
        else
            LogUtils::Info(ss.str());
    }
}
