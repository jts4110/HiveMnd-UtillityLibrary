#pragma once
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <cstdint>

// -----------------------------------------------------------------------------
//  Utility: HiveMnd::Core::DebugUtils
//  Purpose : Provides helpful tools for debugging, tracing, and inspecting
//            memory and runtime state within HiveMnd projects.
//  Notes   : These are developer-only functions; they compile to no-ops in release.
// -----------------------------------------------------------------------------
namespace HiveMnd::Core
{
    class DebugUtils
    {
    public:
        // ---------------------------------------------------------------------
        //  Function: Assert
        //  Purpose : Custom assert that prints message and halts if condition fails.
        //  @param   condition - Boolean expression to test.
        //  @param   message   - Optional message to show on failure.
        // ---------------------------------------------------------------------
        static void Assert(bool condition, const std::string& message = "");

        // ---------------------------------------------------------------------
        //  Function: Trace
        //  Purpose : Prints formatted trace/debug information to console.
        //  @param   message - Text message to output.
        // ---------------------------------------------------------------------
        static void Trace(const std::string& message);

        // ---------------------------------------------------------------------
        //  Function: HexDump
        //  Purpose : Prints a hexdump of raw memory bytes for inspection.
        //  @param   data - Pointer to memory block.
        //  @param   size - Number of bytes to display.
        // ---------------------------------------------------------------------
        static void HexDump(const void* data, size_t size);

        // ---------------------------------------------------------------------
        //  Function: PrintVector
        //  Purpose : Helper to print vector content cleanly for debugging.
        //  @param   vec - Any vector of numeric or string values.
        // ---------------------------------------------------------------------
        template<typename T>
        static void PrintVector(const std::vector<T>& vec)
        {
            std::cout << "[ ";
            for (size_t i = 0; i < vec.size(); ++i)
            {
                std::cout << vec[i];
                if (i < vec.size() - 1)
                    std::cout << ", ";
            }
            std::cout << " ]" << std::endl;
        }
    };

    // -------------------------------------------------------------------------
    //  Macros to simplify debugging usage.
    // -------------------------------------------------------------------------
#ifdef _DEBUG
#define HM_ASSERT(cond, msg) HiveMnd::Core::DebugUtils::Assert((cond), (msg))
#define HM_TRACE(msg) HiveMnd::Core::DebugUtils::Trace(msg)
#else
#define HM_ASSERT(cond, msg)
#define HM_TRACE(msg)
#endif
}

