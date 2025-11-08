#pragma once
#include <string>
#include <vector>
#include <chrono>

// -----------------------------------------------------------------------------
// HiveMnd Utility Library - Network Module
// NetProfiler.h
//
// Purpose:
//     Provides lightweight network performance profiling utilities including
//     ping-like latency tests, connection timing, and bandwidth estimation.
//
// Notes:
//     - Uses cross-platform sockets (Winsock on Windows, BSD sockets otherwise).
//     - Designed for lightweight internal diagnostics, not full ICMP ping.
// -----------------------------------------------------------------------------

namespace HiveMnd
{
    class NetProfiler
    {
    public:
        // Struct for holding a simple ping result
        struct PingResult
        {
            double latencyMs;
            bool success;
        };

        // Runs N test pings to a given host/IP on a given port.
        // Returns a vector of PingResult values for each attempt.
        static std::vector<PingResult> RunPingTest(const std::string& host, int port = 80, int count = 4, int timeoutMs = 1000);

        // Returns the average latency in milliseconds
        static double AverageLatency(const std::vector<PingResult>& results);

        // Returns the packet loss percentage (0–100)
        static double PacketLoss(const std::vector<PingResult>& results);

        // Attempts to estimate connection throughput in KB/s
        static double EstimateDownloadSpeed(const std::string& host, int port = 80, int seconds = 2);
    };
}

