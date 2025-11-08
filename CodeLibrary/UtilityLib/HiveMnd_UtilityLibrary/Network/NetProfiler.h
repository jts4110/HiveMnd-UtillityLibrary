#pragma once
#include <string>
#include <vector>
#include <chrono>

namespace HiveMnd
{
    // Holds ping result data for latency analysis
    struct PingResult
    {
        bool success;
        double latencyMs;
    };

    // Network profiling utilities for latency, packet loss, and throughput
    class NetProfiler
    {
    public:
        static std::vector<PingResult> RunPingTest(const std::string& host, int port = 80, int attempts = 4);
        static double AverageLatency(const std::vector<PingResult>& results);
        static double PacketLoss(const std::vector<PingResult>& results);
        static double EstimateDownloadSpeed(const std::string& host, int port = 80, int seconds = 2);

        // NEW: Logs a full diagnostic report using HiveMnd::Core::LogUtils
        static void LogNetworkReport(const std::string& host, int port = 80);
    };
}
