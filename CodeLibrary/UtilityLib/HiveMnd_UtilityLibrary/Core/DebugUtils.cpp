#include "Core/DebugUtils.h"
#include <chrono>
#include <ctime>

using namespace HiveMnd::Core;

// -----------------------------------------------------------------------------
//  Function: Assert
//  Purpose : Prints failure message and halts program if condition is false.
// -----------------------------------------------------------------------------
void DebugUtils::Assert(bool condition, const std::string& message)
{
    if (!condition)
    {
        std::cerr << "\n[ASSERT FAILED] " << message << std::endl;
#ifdef _WIN32
        __debugbreak(); // Windows-only breakpoint
#else
        std::abort();   // Fallback for non-Windows
#endif
    }
}

// -----------------------------------------------------------------------------
//  Function: Trace
//  Purpose : Prints a formatted debug message with timestamp.
// -----------------------------------------------------------------------------
void DebugUtils::Trace(const std::string& message)
{
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tmBuf{};
#ifdef _WIN32
    localtime_s(&tmBuf, &t);
#else
    localtime_r(&t, &tmBuf);
#endif

    std::ostringstream oss;
    oss << "[" << std::put_time(&tmBuf, "%H:%M:%S") << "] " << message;
    std::cout << oss.str() << std::endl;
}

// -----------------------------------------------------------------------------
//  Function: HexDump
//  Purpose : Display bytes of memory in hex + ASCII side-by-side.
// -----------------------------------------------------------------------------
void DebugUtils::HexDump(const void* data, size_t size)
{
    const unsigned char* bytes = static_cast<const unsigned char*>(data);

    for (size_t i = 0; i < size; i += 16)
    {
        std::cout << std::setw(6) << std::setfill('0') << std::hex << i << "  ";

        // Print hex values
        for (size_t j = 0; j < 16; ++j)
        {
            if (i + j < size)
                std::cout << std::setw(2) << (int)bytes[i + j] << " ";
            else
                std::cout << "   ";
        }

        std::cout << " | ";

        // Print ASCII characters
        for (size_t j = 0; j < 16 && i + j < size; ++j)
        {
            char c = bytes[i + j];
            std::cout << (isprint((unsigned char)c) ? c : '.');
        }
        std::cout << std::endl;
    }

    std::cout << std::dec << std::setfill(' ') << std::endl;
}
