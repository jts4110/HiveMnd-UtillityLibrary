#include "AsyncNet.h"
#include <chrono>

using namespace HiveMnd::Network;
using HiveMnd::Core::LogUtils;

// -----------------------------------------------------------------------------
//  Function: Connect
//  Purpose : Initializes Winsock, creates a TCP socket, connects to the specified
//            host and port, and starts the asynchronous worker thread.
//  Notes   : If the connection fails, it logs an error and returns false.
// -----------------------------------------------------------------------------
bool AsyncNet::Connect(const std::string& host, int port, Callback callback)
{
    WSADATA wsaData;

    // Initialize Winsock safely
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        LogUtils::Error("WSAStartup failed during AsyncNet::Connect.");
        return false;
    }

    mHost = host;
    mPort = port;
    mCallback = callback;

    // Configure target server address
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, host.c_str(), &serverAddr.sin_addr);

    // Create TCP socket
    mSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (mSocket == INVALID_SOCKET)
    {
        LogUtils::Error("Socket creation failed in AsyncNet::Connect.");
        WSACleanup();
        return false;
    }

    // Attempt connection
    if (connect(mSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR)
    {
        LogUtils::Error("Failed to connect to host " + host);
        closesocket(mSocket);
        WSACleanup();
        return false;
    }

    // Launch worker thread
    mRunning = true;
    mThread = std::thread(&AsyncNet::WorkerThread, this);
    LogUtils::Info("AsyncNet connected to " + host + ":" + std::to_string(port));

    return true;
}

// -----------------------------------------------------------------------------
//  Function: Send
//  Purpose : Adds a message to the send queue and notifies the worker thread to
//            process it asynchronously.
// -----------------------------------------------------------------------------
void AsyncNet::Send(const std::string& message)
{
    {
        std::lock_guard<std::mutex> lock(mMutex);
        mSendQueue.push(message);
    }
    mCv.notify_one(); // signal worker thread
}

// -----------------------------------------------------------------------------
//  Function: WorkerThread
//  Purpose : Continuously checks the send queue for messages, sends them, and
//            waits for responses from the server. Uses a condition variable to
//            efficiently sleep when idle.
// -----------------------------------------------------------------------------
void AsyncNet::WorkerThread()
{
    LogUtils::Debug("AsyncNet worker thread started.");

    while (mRunning)
    {
        std::unique_lock<std::mutex> lock(mMutex);
        mCv.wait(lock, [this] { return !mSendQueue.empty() || !mRunning; });

        if (!mRunning)
            break;

        // Get next message from queue
        std::string msg = mSendQueue.front();
        mSendQueue.pop();
        lock.unlock();

        // Send message over the socket
        send(mSocket, msg.c_str(), static_cast<int>(msg.size()), 0);
        LogUtils::Debug("Sent async message: " + msg);

        // Wait for server response
        char buffer[512] = { 0 };
        int bytesReceived = recv(mSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived > 0)
        {
            std::string response(buffer, bytesReceived);
            if (mCallback)
                mCallback(response);
        }
        else if (bytesReceived == 0)
        {
            LogUtils::Warning("Server closed connection.");
            break;
        }
    }

    LogUtils::Info("AsyncNet worker thread exiting...");
}

// -----------------------------------------------------------------------------
//  Function: Stop
//  Purpose : Stops the worker thread, closes the socket, and cleans up Winsock.
// -----------------------------------------------------------------------------
void AsyncNet::Stop()
{
    mRunning = false;
    mCv.notify_all();

    if (mThread.joinable())
        mThread.join();

    if (mSocket != INVALID_SOCKET)
    {
        closesocket(mSocket);
        mSocket = INVALID_SOCKET;
    }

    WSACleanup();
    LogUtils::Info("AsyncNet stopped and cleaned up.");
}

// -----------------------------------------------------------------------------
//  Destructor
//  Purpose : Ensures Stop() is called automatically during cleanup.
// -----------------------------------------------------------------------------
AsyncNet::~AsyncNet()
{
    Stop();
}
