#include "SystemDiagnostics.h"
#include <sstream>
#include <iomanip>

using namespace HiveMnd::Core;

namespace HiveMnd::Diagnostics
{
    std::string SystemInfoSnapshot::ToString() const
    {
        std::ostringstream ss;
        ss << "CPU: " << cpuName << " (" << coreCount << " cores)"
            << "\nRAM: " << std::fixed << std::setprecision(2)
            << (totalRAM / (1024.0 * 1024.0 * 1024.0)) << " GB"
            << "\nOS: " << osVersion;

        if (!gpus.empty())
        {
            ss << "\nGPU(s): ";
            for (const auto& gpu : gpus)
                ss << "\n  - " << gpu;
        }

        return ss.str();
    }

#ifdef _WIN32
    std::string SystemDiagnostics::GetCPUName()
    {
        char cpuName[128] = { 0 };
        int cpuInfo[4] = { -1 };
        __cpuid(cpuInfo, 0x80000002);
        memcpy(cpuName, cpuInfo, sizeof(cpuInfo));
        __cpuid(cpuInfo, 0x80000003);
        memcpy(cpuName + 16, cpuInfo, sizeof(cpuInfo));
        __cpuid(cpuInfo, 0x80000004);
        memcpy(cpuName + 32, cpuInfo, sizeof(cpuInfo));
        return std::string(cpuName);
    }

    unsigned long long SystemDiagnostics::GetTotalRAM()
    {
        MEMORYSTATUSEX memInfo{};
        memInfo.dwLength = sizeof(memInfo);
        GlobalMemoryStatusEx(&memInfo);
        return memInfo.ullTotalPhys;
    }

    std::vector<std::string> SystemDiagnostics::GetGPUList()
    {
        std::vector<std::string> gpus;
        IDXGIFactory* pFactory = nullptr;
        if (SUCCEEDED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory)))
        {
            IDXGIAdapter* pAdapter = nullptr;
            for (UINT i = 0; pFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND; ++i)
            {
                DXGI_ADAPTER_DESC desc;
                pAdapter->GetDesc(&desc);
                char gpuName[128];
                wcstombs_s(nullptr, gpuName, desc.Description, 128);
                gpus.emplace_back(gpuName);
                pAdapter->Release();
            }
            pFactory->Release();
        }
        return gpus;
    }

    std::string SystemDiagnostics::GetWindowsVersion()
    {
        if (IsWindows10OrGreater()) return "Windows 10/11";
        if (IsWindows8Point1OrGreater()) return "Windows 8.1";
        if (IsWindows8OrGreater()) return "Windows 8";
        if (IsWindows7OrGreater()) return "Windows 7";
        return "Unknown Windows Version";
    }
#endif

    SystemInfoSnapshot SystemDiagnostics::Collect()
    {
        std::lock_guard<std::mutex> lock(sMutex);
        SystemInfoSnapshot info{};

#ifdef _WIN32
        info.cpuName = GetCPUName();
        info.coreCount = std::thread::hardware_concurrency();
        info.totalRAM = GetTotalRAM();
        info.gpus = GetGPUList();
        info.osVersion = GetWindowsVersion();
#else
        info.cpuName = "Unknown CPU";
        info.coreCount = std::thread::hardware_concurrency();
        info.totalRAM = 0;
        info.gpus = { "Unknown GPU" };
        info.osVersion = "Non-Windows System";
#endif

        return info;
    }

    void SystemDiagnostics::LogSummary()
    {
        SystemInfoSnapshot info = Collect();
        LogUtils::Info("=== System Diagnostics Summary ===");
        LogUtils::Info(info.ToString());
    }
}
