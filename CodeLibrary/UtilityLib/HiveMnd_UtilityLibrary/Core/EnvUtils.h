#pragma once
#include <string>
#include <vector>

// -----------------------------------------------------------------------------
//  Utility: HiveMnd::Core::EnvUtils
//  Purpose : Provides system environment variable handling and runtime info
//            such as OS type, architecture, and processor count.
//  Notes   : All functions are static and platform-safe.
// -----------------------------------------------------------------------------
namespace HiveMnd::Core
{
    class EnvUtils
    {
    public:
        // ---------------------------------------------------------------------
        //  Function: GetEnv
        //  Purpose : Retrieves the value of a system environment variable.
        //  @param   name - Name of the environment variable.
        //  @return  Value of the variable, or empty string if not found.
        // ---------------------------------------------------------------------
        static std::string GetEnv(const std::string& name);

        // ---------------------------------------------------------------------
        //  Function: SetEnv
        //  Purpose : Creates or updates a system environment variable.
        //  @param   name  - Variable name.
        //  @param   value - Variable value.
        //  @return  True if successful, otherwise false.
        // ---------------------------------------------------------------------
        static bool SetEnv(const std::string& name, const std::string& value);

        // ---------------------------------------------------------------------
        //  Function: GetAllEnv
        //  Purpose : Returns a list of all available environment variables.
        //  @return  A vector containing all environment variable entries.
        // ---------------------------------------------------------------------
        static std::vector<std::string> GetAllEnv();

        // ---------------------------------------------------------------------
        //  Function: GetOSName
        //  Purpose : Detects and returns the operating system name.
        //  @return  A human-readable string of the OS.
        // ---------------------------------------------------------------------
        static std::string GetOSName();

        // ---------------------------------------------------------------------
        //  Function: GetProcessorCount
        //  Purpose : Returns number of logical processors available.
        //  @return  Integer count of CPU threads.
        // ---------------------------------------------------------------------
        static int GetProcessorCount();

        // ---------------------------------------------------------------------
        //  Function: Is64Bit
        //  Purpose : Determines if the current process is 64-bit.
        //  @return  True if running as 64-bit, false if 32-bit.
        // ---------------------------------------------------------------------
        static bool Is64Bit();
    };
}

