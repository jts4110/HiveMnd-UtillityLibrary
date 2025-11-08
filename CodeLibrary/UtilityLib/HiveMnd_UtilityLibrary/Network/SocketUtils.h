// -----------------------------------------------------------------------------
// HiveMnd Utility Library : SocketUtils
// -----------------------------------------------------------------------------
// Overview:
//  SocketUtils provides a safe, cross-platform wrapper around low-level socket
//  functions (TCP and UDP). This allows developers to create lightweight
//  networking layers for real-time communication, remote logging, and testing.
//
// Key Features:
//  • Cross-platform TCP and UDP socket support
//  • Simple send/receive with timeout handling
//  • Reusable connection abstraction
//  • Automatic cleanup via RAII
//
// Usage:
//  #include "Network/SocketUtils.h"
//  HiveMnd::Network::Socket client;
//  if (client.Connect("example.com", 80)) client.Send("Hello");
// -----------------------------------------------------------------------------

#pragma once
#include <string>
#include <vector>

namespace HiveMnd::Network
{
    enum class SocketProtocol
    {
        TCP,
        UDP
    };

    class Socket
    {
    private:
#ifdef _WIN32
        using SocketHandle = unsigned long long;
#else
        using SocketHandle = int;
#endif
        SocketHandle mSocket;
        bool mConnected;
        SocketProtocol mProtocol;

    public:
        Socket(SocketProtocol protocol = SocketProtocol::TCP);
        ~Socket();

        // Establish connection (for TCP)
        bool Connect(const std::string& host, int port);

        // Bind and listen (for servers)
        bool Bind(int port);
        bool Listen(int backlog = 5);
        Socket* Accept();

        // Data send/receive
        bool Send(const std::string& data);
        int Receive(std::string& outData, int maxBytes = 4096);

        // UDP-specific send/receive
        bool SendTo(const std::string& host, int port, const std::string& data);
        int ReceiveFrom(std::string& outData, std::string& fromHost, int& fromPort);

        // Close connection
        void Close();

        // Utility checks
        bool IsConnected() const { return mConnected; }
        SocketProtocol GetProtocol() const { return mProtocol; }
    };
}

