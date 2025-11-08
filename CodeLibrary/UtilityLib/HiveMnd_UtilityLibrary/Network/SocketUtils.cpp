#include "SocketUtils.h"

#include <iostream>
#include <cstring>
#include <sstream>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#endif

namespace HiveMnd::Network
{
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
    // Constructor
    // -------------------------------------------------------------------------
    Socket::Socket(SocketProtocol protocol)
        : mConnected(false), mProtocol(protocol)
    {
        InitSocketLayer();
        mSocket = socket(AF_INET,
            (protocol == SocketProtocol::TCP) ? SOCK_STREAM : SOCK_DGRAM,
            (protocol == SocketProtocol::TCP) ? IPPROTO_TCP : IPPROTO_UDP);
        if (mSocket <= 0)
            std::cerr << "[SocketUtils] Failed to create socket.\n";
    }

    // -------------------------------------------------------------------------
    // Destructor
    // -------------------------------------------------------------------------
    Socket::~Socket()
    {
        Close();
    }

    // -------------------------------------------------------------------------
    // Connect (TCP)
    // -------------------------------------------------------------------------
    bool Socket::Connect(const std::string& host, int port)
    {
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);

        if (inet_pton(AF_INET, host.c_str(), &addr.sin_addr) <= 0)
        {
            hostent* he = gethostbyname(host.c_str());
            if (!he)
                return false;
            memcpy(&addr.sin_addr, he->h_addr_list[0], he->h_length);
        }

        if (connect(mSocket, (sockaddr*)&addr, sizeof(addr)) == 0)
        {
            mConnected = true;
            return true;
        }
        return false;
    }

    // -------------------------------------------------------------------------
    // Bind and Listen (for server)
    // -------------------------------------------------------------------------
    bool Socket::Bind(int port)
    {
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(port);
        return bind(mSocket, (sockaddr*)&addr, sizeof(addr)) == 0;
    }

    bool Socket::Listen(int backlog)
    {
        return listen(mSocket, backlog) == 0;
    }

    Socket* Socket::Accept()
    {
        sockaddr_in client{};
        socklen_t len = sizeof(client);
        int newSock = accept(mSocket, (sockaddr*)&client, &len);
        if (newSock < 0)
            return nullptr;

        Socket* clientSock = new Socket(SocketProtocol::TCP);
        clientSock->mSocket = newSock;
        clientSock->mConnected = true;
        return clientSock;
    }

    // -------------------------------------------------------------------------
    // Send / Receive (TCP)
    // -------------------------------------------------------------------------
    bool Socket::Send(const std::string& data)
    {
        if (!mConnected)
            return false;
        int sent = send(mSocket, data.c_str(), (int)data.size(), 0);
        return sent == (int)data.size();
    }

    int Socket::Receive(std::string& outData, int maxBytes)
    {
        if (!mConnected)
            return -1;
        std::vector<char> buffer(maxBytes);
        int bytesRead = recv(mSocket, buffer.data(), maxBytes, 0);
        if (bytesRead > 0)
            outData.assign(buffer.begin(), buffer.begin() + bytesRead);
        return bytesRead;
    }

    // -------------------------------------------------------------------------
    // UDP send/receive
    // -------------------------------------------------------------------------
    bool Socket::SendTo(const std::string& host, int port, const std::string& data)
    {
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        inet_pton(AF_INET, host.c_str(), &addr.sin_addr);
        int sent = sendto(mSocket, data.c_str(), (int)data.size(), 0,
            (sockaddr*)&addr, sizeof(addr));
        return sent == (int)data.size();
    }

    int Socket::ReceiveFrom(std::string& outData, std::string& fromHost, int& fromPort)
    {
        sockaddr_in sender{};
        socklen_t len = sizeof(sender);
        std::vector<char> buffer(4096);
        int bytes = recvfrom(mSocket, buffer.data(), (int)buffer.size(), 0,
            (sockaddr*)&sender, &len);
        if (bytes > 0)
        {
            outData.assign(buffer.begin(), buffer.begin() + bytes);
            char ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &sender.sin_addr, ip, sizeof(ip));
            fromHost = ip;
            fromPort = ntohs(sender.sin_port);
        }
        return bytes;
    }

    // -------------------------------------------------------------------------
    // Close connection
    // -------------------------------------------------------------------------
    void Socket::Close()
    {
        if (mSocket > 0)
        {
#ifdef _WIN32
            closesocket(mSocket);
#else
            close(mSocket);
#endif
            mConnected = false;
        }
    }
}
