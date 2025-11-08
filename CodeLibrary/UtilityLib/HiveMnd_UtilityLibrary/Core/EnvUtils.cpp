#include "Core/EnvUtils.h"
#include <cstdlib>
#include <thread>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

using namespace HiveMnd::Core;

// -----------------------------------------------------------------------------
//  Function: GetEnv
//  Purpose : Retrieve a single environment variable's value.
// -----------------------------------------------------------------------------
std::string EnvUtils::GetEnv(const std::string& name)
{
    const char* val = std::getenv(name.c_str());
    return (val ? std::string(val) : "");
}

// -----------------------------------------------------------------------------
//  Function: SetEnv
//  Purpose : Set or update an environment variable.
// -----------------------------------------------------------------------------
bool EnvUtils::SetEnv(const std::string& name, const std::string& value)
{
#ifdef _WIN32
    return _putenv_s(name.c_str(), value.c_str()) == 0;
#else
    return setenv(name.c_str(), value.c_str(), 1) == 0;
#endif
}

// -----------------------------------------------------------------------------
//  Function: GetAllEnv
//  Purpose : Collects all environment variables available to the process.
// -----------------------------------------------------------------------------
std::vector<std::string> EnvUtils::GetAllEnv()
{
    std::vector<std::string> vars;

#ifdef _WIN32
    LPWCH env = GetEnvironmentStringsW();
    if (!env) return vars;

    LPWCH current = env;
    while (*current)
    {
        std::wstring wvar(current);
        std::string var(wvar.begin(), wvar.end());
        vars.push_back(var);
        current += wvar.size() + 1;
    }
    FreeEnvironmentStringsW(env);
#else
    extern char** environ;
    for (int i = 0; environ[i] != nullptr; ++i)
    {
        vars.emplace_back(environ[i]);
    }
#endif
    return vars;
}

// -----------------------------------------------------------------------------
//  Function: GetOSName
//  Purpose : Identify current operating system name.
// -----------------------------------------------------------------------------
std::string EnvUtils::GetOSName()
{
#ifdef _WIN32
    return "Windows";
#elif __APPLE__
    return "macOS";
#elif __linux__
    return "Linux";
#elif __unix__
    return "Unix";
#else
    return "Unknown OS";
#endif
}

// -----------------------------------------------------------------------------
//  Function: GetProcessorCount
//  Purpose : Retrieves number of CPU cores / logical processors.
// -----------------------------------------------------------------------------
int EnvUtils::GetProcessorCount()
{
    unsigned int count = std::thread::hardware_concurrency();
    return count == 0 ? 1 : static_cast<int>(count);
}

// -----------------------------------------------------------------------------
//  Function: Is64Bit
//  Purpose : Determines if process is 64-bit.
// -----------------------------------------------------------------------------
bool EnvUtils::Is64Bit()
{
#if defined(_WIN64) || defined(__x86_64__) || defined(__ppc64__)
    return true;
#else
    return false;
#endif
}
