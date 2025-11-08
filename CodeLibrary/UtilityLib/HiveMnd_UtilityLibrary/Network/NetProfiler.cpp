#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "NetProfiler.h"
#include "../Core/LogUtils.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <numeric>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

using namespace HiveMnd;

std::vector<PingResult> NetProfiler::RunPingTest(const std::string& host, int port, int attempts)
{
    std::vector<PingResult> results;
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        HiveMnd::Core::LogUtils::Error("WSAStartup failed to initialize Winsock for RunPingTest.");
        return {}; // return an empty vector
    }


    for (int i = 0; i < attempts; ++i)
    {
        auto start = std::chrono::high_resolution_clock::now();

        SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
        sockaddr_in server;
        server.sin_family = AF_INET;
        server.sin_port = htons(port);
        server.sin_addr.s_addr = inet_addr(host.c_str());

        bool success = connect(sock, (sockaddr*)&server, sizeof(server)) != SOCKET_ERROR;
        closesocket(sock);

        auto end = std::chrono::high_resolution_clock::now();
        double latency = std::chrono::duration<double, std::milli>(end - start).count();

        results.push_back({ success, latency });
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    WSACleanup();
    return results;
}

double NetProfiler::AverageLatency(const std::vector<PingResult>& results)
{
    double total = 0.0;
    int count = 0;
    for (auto& r : results)
    {
        if (r.success)
        {
            total += r.latencyMs;
            ++count;
        }
    }
    return count > 0 ? total / count : 0.0;
}

double NetProfiler::PacketLoss(const std::vector<PingResult>& results)
{
    int failed = 0;
    for (auto& r : results)
        if (!r.success)
            ++failed;
    return (static_cast<double>(failed) / results.size()) * 100.0;
}

double NetProfiler::EstimateDownloadSpeed(const std::string& host, int port, int seconds)
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        HiveMnd::Core::LogUtils::Error("WSAStartup failed to initialize Winsock for RunPingTest.");
        return {}; // return an empty vector
    }


    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(host.c_str());

    if (connect(sock, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
    {
        closesocket(sock);
        WSACleanup();
        return 0.0;
    }

    const int bufferSize = 4096;
    char buffer[bufferSize];
    int totalBytes = 0;
    auto start = std::chrono::high_resolution_clock::now();

    while (std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - start).count() < seconds)
    {
        int bytes = recv(sock, buffer, bufferSize, 0);
        if (bytes <= 0) break;
        totalBytes += bytes;
    }

    closesocket(sock);
    WSACleanup();

    return (totalBytes / 1024.0) / seconds; // KB/s
}

// -----------------------------------------------------------------------------
// LogNetworkReport() — integrates profiling results with HiveMnd::Core::LogUtils
// -----------------------------------------------------------------------------
void NetProfiler::LogNetworkReport(const std::string& host, int port)
{
    HiveMnd::Core::LogUtils::Info("Starting network diagnostics for host: " + host);

    auto results = HiveMnd::NetProfiler::RunPingTest(host, port);
    double avg = HiveMnd::NetProfiler::AverageLatency(results);
    double loss = HiveMnd::NetProfiler::PacketLoss(results);

    for (size_t i = 0; i < results.size(); ++i)
    {
        std::string msg = "Ping " + std::to_string(i + 1) + ": ";
        msg += results[i].success ? std::to_string(results[i].latencyMs) + " ms" : "Failed";
        HiveMnd::Core::LogUtils::Debug(msg);
    }

    HiveMnd::Core::LogUtils::Info("Average Latency: " + std::to_string(avg) + " ms");
    HiveMnd::Core::LogUtils::Info("Packet Loss: " + std::to_string(loss) + "%");

    double speed = HiveMnd::NetProfiler::EstimateDownloadSpeed(host, port, 2);
    HiveMnd::Core::LogUtils::Info("Estimated Download Speed: " + std::to_string(speed) + " KB/s");

    HiveMnd::Core::LogUtils::Success("Network diagnostics completed for host: " + host);
}

