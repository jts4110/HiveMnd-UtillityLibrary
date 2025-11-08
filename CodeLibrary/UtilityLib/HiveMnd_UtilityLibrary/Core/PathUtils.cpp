#include "Core/PathUtils.h"
#include <filesystem>
#include <algorithm>

// Use C++17 filesystem for robust path management
namespace fs = std::filesystem;
using namespace HiveMnd::Core;

// -----------------------------------------------------------------------------
//  Function: Normalize
//  Purpose : Converts all slashes to platform-preferred style.
// -----------------------------------------------------------------------------
std::string PathUtils::Normalize(const std::string& path)
{
    std::string result = path;
#ifdef _WIN32
    std::replace(result.begin(), result.end(), '/', '\\');
#else
    std::replace(result.begin(), result.end(), '\\', '/');
#endif
    return result;
}

// -----------------------------------------------------------------------------
//  Function: Join
//  Purpose : Combines path segments into one valid path string.
// -----------------------------------------------------------------------------
std::string PathUtils::Join(const std::vector<std::string>& parts)
{
    if (parts.empty()) return "";

    fs::path combined(parts[0]);
    for (size_t i = 1; i < parts.size(); ++i)
        combined /= parts[i];

    return combined.string();
}

// -----------------------------------------------------------------------------
//  Function: GetFileName
//  Purpose : Extracts the file name from a full path.
// -----------------------------------------------------------------------------
std::string PathUtils::GetFileName(const std::string& path)
{
    return fs::path(path).filename().string();
}

// -----------------------------------------------------------------------------
//  Function: GetExtension
//  Purpose : Returns the file extension (with dot if present).
// -----------------------------------------------------------------------------
std::string PathUtils::GetExtension(const std::string& path)
{
    return fs::path(path).extension().string();
}

// -----------------------------------------------------------------------------
//  Function: GetDirectory
//  Purpose : Returns the directory portion of a path.
// -----------------------------------------------------------------------------
std::string PathUtils::GetDirectory(const std::string& path)
{
    return fs::path(path).parent_path().string();
}

// -----------------------------------------------------------------------------
//  Function: Exists
//  Purpose : Returns true if a file or folder exists at the given path.
// -----------------------------------------------------------------------------
bool PathUtils::Exists(const std::string& path)
{
    return fs::exists(fs::path(path));
}
