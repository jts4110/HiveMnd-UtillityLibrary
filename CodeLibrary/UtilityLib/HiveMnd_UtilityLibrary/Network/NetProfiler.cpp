#include "NetProfiler.h"

#ifdef _WIN32
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <arpa/inet.h>
#include <unistd.h>
#endif

#include <iostream>
#include <cstring>

namespace HiveMnd
{
    // Helper to ensure Winsock is initialized (Windows only)
    static void InitSockets()
    {
#ifdef _WIN32
        static bool initialized = false;
        if (!initialized)
        {
            WSADATA wsa;
            WSAStartup(MAKEWORD(2, 2), &wsa);
            initialized = true;
        }
#endif
    }

    std::vector<NetProfiler::PingResult> NetProfiler::RunPingTest(const std::string& host, int port, int count, int timeoutMs)
    {
        InitSockets();
        std::vector<PingResult> results;
        results.reserve(count);

        for (int i = 0; i < count; ++i)
        {
            SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
            if (sock == INVALID_SOCKET)
            {
                results.push_back({ 0, false });
                continue;
            }

            sockaddr_in server{};
            server.sin_family = AF_INET;
            server.sin_port = htons(port);
            server.sin_addr.s_addr = inet_addr(host.c_str());

            auto start = std::chrono::high_resolution_clock::now();
            bool success = (connect(sock, (sockaddr*)&server, sizeof(server)) == 0);
            auto end = std::chrono::high_resolution_clock::now();

            std::chrono::duration<double, std::milli> diff = end - start;
            results.push_back({ diff.count(), success });

#ifdef _WIN32
            closesocket(sock);
#else
            close(sock);
#endif
        }

        return results;
    }

    double NetProfiler::AverageLatency(const std::vector<PingResult>& results)
    {
        if (results.empty()) return 0.0;
        double sum = 0.0;
        int valid = 0;
        for (const auto& r : results)
        {
            if (r.success)
            {
                sum += r.latencyMs;
                ++valid;
            }
        }
        return valid > 0 ? (sum / valid) : 0.0;
    }

    double NetProfiler::PacketLoss(const std::vector<PingResult>& results)
    {
        if (results.empty()) return 100.0;
        int lost = 0;
        for (const auto& r : results)
            if (!r.success) ++lost;
        return (static_cast<double>(lost) / results.size()) * 100.0;
    }

    double NetProfiler::EstimateDownloadSpeed(const std::string& host, int port, int seconds)
    {
        InitSockets();

        SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == INVALID_SOCKET)
            return 0.0;

        sockaddr_in server{};
        server.sin_family = AF_INET;
        server.sin_port = htons(port);
        server.sin_addr.s_addr = inet_addr(host.c_str());

        if (connect(sock, (sockaddr*)&server, sizeof(server)) != 0)
        {
#ifdef _WIN32
            closesocket(sock);
#else
            close(sock);
#endif
            return 0.0;
        }

        // Send a small HTTP GET or dummy data
        const char* request = "GET / HTTP/1.1\r\nHost: test\r\nConnection: close\r\n\r\n";
        send(sock, request, static_cast<int>(strlen(request)), 0);

        const int bufSize = 4096;
        char buffer[bufSize];
        int totalBytes = 0;
        auto start = std::chrono::high_resolution_clock::now();

        while (true)
        {
            int bytes = recv(sock, buffer, bufSize, 0);
            if (bytes <= 0) break;
            totalBytes += bytes;

            auto now = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = now - start;
            if (elapsed.count() >= seconds) break;
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> total = end - start;
        double kbPerSec = (totalBytes / 1024.0) / total.count();

#ifdef _WIN32
        closesocket(sock);
#else
        close(sock);
#endif

        return kbPerSec;
    }
}
