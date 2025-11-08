#include "AdvancedNet.h"
#include "NetworkUtils.h"

#include <iostream>
#include <sstream>
#include <chrono>
#include <cmath>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#endif

namespace HiveMnd::Network
{
    // -------------------------------------------------------------------------
    // Helper: Initialize Winsock for Windows
    // -------------------------------------------------------------------------
    static void InitSocketLayer()
    {
#ifdef _WIN32
        static bool initialized = false;
        if (!initialized)
        {
            WSADATA wsaData;
            if (WSAStartup(MAKEWORD(2, 2), &wsaData) == 0)
                initialized = true;
        }
#endif
    }

    // -------------------------------------------------------------------------
    // GetPingLatency() : Measures latency to host (ms)
    // -------------------------------------------------------------------------
    double GetPingLatency(const std::string& host, int port, int attempts)
    {
        InitSocketLayer();

        addrinfo hints{}, * info = nullptr;
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;

        if (getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hints, &info) != 0)
            return -1.0;

        double totalMs = 0;
        int successCount = 0;

        for (int i = 0; i < attempts; ++i)
        {
            auto start = std::chrono::high_resolution_clock::now();

            SOCKET s = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
            if (s == INVALID_SOCKET)
                continue;

            int rc = connect(s, info->ai_addr, (int)info->ai_addrlen);
            auto end = std::chrono::high_resolution_clock::now();

            if (rc == 0)
            {
                std::chrono::duration<double, std::milli> elapsed = end - start;
                totalMs += elapsed.count();
                successCount++;
            }

#ifdef _WIN32
            closesocket(s);
#else
            close(s);
#endif
        }

        freeaddrinfo(info);
        if (successCount == 0) return -1.0;
        return totalMs / successCount;
    }

    // -------------------------------------------------------------------------
    // DNSLookup() : Resolves hostname → IP list
    // -------------------------------------------------------------------------
    std::vector<std::string> DNSLookup(const std::string& hostname)
    {
        InitSocketLayer();
        std::vector<std::string> results;

        addrinfo* res = nullptr;
        if (getaddrinfo(hostname.c_str(), nullptr, nullptr, &res) == 0)
        {
            for (auto p = res; p != nullptr; p = p->ai_next)
            {
                char ipStr[INET6_ADDRSTRLEN];
                void* addrPtr = nullptr;

                if (p->ai_family == AF_INET)
                    addrPtr = &((sockaddr_in*)p->ai_addr)->sin_addr;
                else if (p->ai_family == AF_INET6)
                    addrPtr = &((sockaddr_in6*)p->ai_addr)->sin6_addr;

                if (addrPtr)
                {
                    inet_ntop(p->ai_family, addrPtr, ipStr, sizeof(ipStr));
                    results.emplace_back(ipStr);
                }
            }
            freeaddrinfo(res);
        }
        return results;
    }

    // -------------------------------------------------------------------------
    // ReverseDNS() : Resolves IP → hostname
    // -------------------------------------------------------------------------
    std::string ReverseDNS(const std::string& ip)
    {
        InitSocketLayer();
        sockaddr_in sa{};
        sa.sin_family = AF_INET;
        inet_pton(AF_INET, ip.c_str(), &sa.sin_addr);

        char host[NI_MAXHOST];
        if (getnameinfo((sockaddr*)&sa, sizeof(sa), host, sizeof(host), nullptr, 0, 0) == 0)
            return std::string(host);

        return "Unresolved";
    }

    // -------------------------------------------------------------------------
    // MeasureDownloadSpeed() : crude throughput test using socket read
    // -------------------------------------------------------------------------
    double MeasureDownloadSpeed(const std::string& url, size_t maxBytes)
    {
        InitSocketLayer();

        std::string host = url;
        std::string path = "/";
        if (url.find("http://") == 0)
            host = url.substr(7);
        if (auto slash = host.find('/'); slash != std::string::npos)
        {
            path = host.substr(slash);
            host = host.substr(0, slash);
        }

        addrinfo hints{}, * info = nullptr;
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;

        if (getaddrinfo(host.c_str(), "80", &hints, &info) != 0)
            return -1.0;

        SOCKET s = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
        if (s == INVALID_SOCKET)
            return -1.0;

        if (connect(s, info->ai_addr, (int)info->ai_addrlen) != 0)
        {
#ifdef _WIN32
            closesocket(s);
#else
            close(s);
#endif
            freeaddrinfo(info);
            return -1.0;
        }

        std::ostringstream req;
        req << "GET " << path << " HTTP/1.1\r\n"
            << "Host: " << host << "\r\n"
            << "Connection: close\r\n\r\n";
        std::string request = req.str();
        send(s, request.c_str(), (int)request.size(), 0);

        auto start = std::chrono::high_resolution_clock::now();

        char buffer[4096];
        size_t totalBytes = 0;
        int bytesRead = 0;
        while ((bytesRead = recv(s, buffer, sizeof(buffer), 0)) > 0)
        {
            totalBytes += bytesRead;
            if (totalBytes >= maxBytes)
                break;
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;

        double kbps = (totalBytes / 1024.0) / elapsed.count();

#ifdef _WIN32
        closesocket(s);
#else
        close(s);
#endif
        freeaddrinfo(info);
        return kbps;
    }

    // -------------------------------------------------------------------------
    // EncodeData() / DecodeData() : Simple base64-like encoding
    // -------------------------------------------------------------------------
    static const char* BASE64_CHARS =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    std::string EncodeData(const std::vector<unsigned char>& data)
    {
        std::string out;
        int val = 0, valb = -6;
        for (unsigned char c : data)
        {
            val = (val << 8) + c;
            valb += 8;
            while (valb >= 0)
            {
                out.push_back(BASE64_CHARS[(val >> valb) & 0x3F]);
                valb -= 6;
            }
        }
        if (valb > -6)
            out.push_back(BASE64_CHARS[((val << 8) >> (valb + 8)) & 0x3F]);
        while (out.size() % 4)
            out.push_back('=');
        return out;
    }

    std::vector<unsigned char> DecodeData(const std::string& encoded)
    {
        std::vector<int> T(256, -1);
        for (int i = 0; i < 64; i++)
            T[BASE64_CHARS[i]] = i;

        std::vector<unsigned char> out;
        int val = 0, valb = -8;
        for (unsigned char c : encoded)
        {
            if (T[c] == -1) break;
            val = (val << 6) + T[c];
            valb += 6;
            if (valb >= 0)
            {
                out.push_back(char((val >> valb) & 0xFF));
                valb -= 8;
            }
        }
        return out;
    }
}
