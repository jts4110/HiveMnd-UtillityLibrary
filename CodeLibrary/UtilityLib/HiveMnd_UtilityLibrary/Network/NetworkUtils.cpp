#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "NetworkUtils.h"

#include <iostream>
#include <sstream>
#include <cstring>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")
#else
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#endif

namespace HiveMnd::Network
{
    // -------------------------------------------------------------------------
    // Initialize Winsock (Windows only)
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
    // GetHostName() : Returns the computer’s hostname
    // -------------------------------------------------------------------------
    std::string GetHostName()
    {
        InitSocketLayer();
        char hostname[256];
        if (gethostname(hostname, sizeof(hostname)) == 0)
            return std::string(hostname);
        return "UnknownHost";
    }

    // -------------------------------------------------------------------------
    // GetLocalIPAddress() : Returns the primary local IPv4 address
    // -------------------------------------------------------------------------
    std::string GetLocalIPAddress()
    {
        InitSocketLayer();

        char hostname[256];
        if (gethostname(hostname, sizeof(hostname)) != 0)
            return "Unknown";

        addrinfo hints{}, * info = nullptr;
        hints.ai_family = AF_INET; // IPv4 only

        if (getaddrinfo(hostname, nullptr, &hints, &info) != 0)
            return "Unavailable";

        std::string ip = "Unavailable";
        for (addrinfo* p = info; p != nullptr; p = p->ai_next)
        {
            sockaddr_in* addr = reinterpret_cast<sockaddr_in*>(p->ai_addr);
            ip = inet_ntoa(addr->sin_addr);
            break;
        }
        freeaddrinfo(info);
        return ip;
    }

    // -------------------------------------------------------------------------
    // GetAllLocalIPs() : Returns a list of all active local IP addresses
    // -------------------------------------------------------------------------
    std::vector<std::string> GetAllLocalIPs()
    {
        InitSocketLayer();
        std::vector<std::string> result;

#ifdef _WIN32
        ULONG bufLen = 15000;
        std::vector<char> buffer(bufLen);
        PIP_ADAPTER_ADDRESSES addresses = reinterpret_cast<PIP_ADAPTER_ADDRESSES>(buffer.data());

        if (GetAdaptersAddresses(AF_INET, 0, nullptr, addresses, &bufLen) == NO_ERROR)
        {
            for (auto addr = addresses; addr != nullptr; addr = addr->Next)
            {
                for (auto unicast = addr->FirstUnicastAddress; unicast != nullptr; unicast = unicast->Next)
                {
                    sockaddr_in* sa_in = reinterpret_cast<sockaddr_in*>(unicast->Address.lpSockaddr);
                    char ip[INET_ADDRSTRLEN];
                    inet_ntop(AF_INET, &(sa_in->sin_addr), ip, INET_ADDRSTRLEN);
                    result.emplace_back(ip);
                }
            }
        }
#else
        ifaddrs* ifAddrStruct = nullptr;
        getifaddrs(&ifAddrStruct);

        for (ifaddrs* ifa = ifAddrStruct; ifa != nullptr; ifa = ifa->ifa_next)
        {
            if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET)
            {
                void* addrPtr = &((sockaddr_in*)ifa->ifa_addr)->sin_addr;
                char ip[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, addrPtr, ip, INET_ADDRSTRLEN);
                result.emplace_back(ip);
            }
        }
        if (ifAddrStruct) freeifaddrs(ifAddrStruct);
#endif

        return result;
    }

    // -------------------------------------------------------------------------
    // IsInternetAvailable() : Simple DNS resolution check
    // -------------------------------------------------------------------------
    bool IsInternetAvailable()
    {
        InitSocketLayer();
        addrinfo* result = nullptr;
        bool success = (getaddrinfo("www.google.com", nullptr, nullptr, &result) == 0);
        if (result)
            freeaddrinfo(result);
        return success;
    }

    // -------------------------------------------------------------------------
    // PingHost() : Basic host reachability test
    // -------------------------------------------------------------------------
    bool PingHost(const std::string& host)
    {
        InitSocketLayer();
        addrinfo* result = nullptr;
        bool reachable = false;

        if (getaddrinfo(host.c_str(), "80", nullptr, &result) == 0)
        {
            SOCKET s = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
            if (s != INVALID_SOCKET)
            {
                int rc = connect(s, result->ai_addr, (int)result->ai_addrlen);
                reachable = (rc == 0);
#ifdef _WIN32
                closesocket(s);
#else
                close(s);
#endif
            }
            freeaddrinfo(result);
        }
        return reachable;
    }
}
