#pragma once
#include <string>
#include <thread>
#include <atomic>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "../Core/LogUtils.h"

#pragma comment(lib, "Ws2_32.lib")

namespace HiveMnd::Network
{
    // -----------------------------------------------------------------------------
    //  Class: AsyncNet
    //  Purpose : Provides a simple asynchronous networking interface for sending
    //            and receiving TCP messages in the background using a worker thread.
    //            Messages are queued and sent non-blockingly, and responses can
    //            trigger optional callbacks for real-time handling.
    //
    //  Notes :
    //    • Uses standard C++ threads and synchronization for thread safety.
    //    • Intended for lightweight asynchronous communication.
    //    • Requires Windows sockets (Winsock2).
    // -----------------------------------------------------------------------------
    class AsyncNet
    {
    public:
        // Callback type used for handling server responses.
        using Callback = std::function<void(const std::string& response)>;

    private:
        std::thread mThread;                      // Background worker thread
        std::atomic<bool> mRunning{ false };      // Controls thread loop state
        std::queue<std::string> mSendQueue;       // Queue of outbound messages
        std::mutex mMutex;                        // Thread lock for queue access
        std::condition_variable mCv;              // Signals when data is ready
        Callback mCallback;                       // Optional response callback

        SOCKET mSocket = INVALID_SOCKET;          // Active socket connection
        std::string mHost;                        // Remote host IP or domain
        int mPort = 0;                            // Remote port number

        // Internal method executed by the worker thread
        void WorkerThread();

    public:
        AsyncNet() = default;
        ~AsyncNet();

        // -------------------------------------------------------------------------
        //  Function: Connect
        //  Purpose : Establishes a connection to the given host and port, and starts
        //            the worker thread for asynchronous communication.
        //  @param host - IP address or domain name to connect to.
        //  @param port - Port number for the connection.
        //  @param callback - Function to be called when data is received.
        //  @return true if connection succeeds, false otherwise.
        // -------------------------------------------------------------------------
        bool Connect(const std::string& host, int port, Callback callback);

        // -------------------------------------------------------------------------
        //  Function: Send
        //  Purpose : Queues a message to be sent to the connected host asynchronously.
        //  @param message - The string message to send.
        // -------------------------------------------------------------------------
        void Send(const std::string& message);

        // -------------------------------------------------------------------------
        //  Function: Stop
        //  Purpose : Gracefully stops the background thread, closes the socket, and
        //            cleans up Winsock resources.
        // -------------------------------------------------------------------------
        void Stop();
    };
}

