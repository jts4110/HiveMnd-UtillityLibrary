#pragma once
#include <string>
#include <vector>

// -----------------------------------------------------------------------------
//  Utility: HiveMnd::Core::PathUtils
//  Purpose : Provides cross-platform path handling tools including normalization,
//            joining, splitting, and extracting filenames or extensions.
//  Notes   : All functions are static and lightweight — no heap allocation.
// -----------------------------------------------------------------------------
namespace HiveMnd::Core
{
    class PathUtils
    {
    public:
        // ---------------------------------------------------------------------
        //  Function: Normalize
        //  Purpose : Converts all slashes to platform-preferred style ('\\' on Windows).
        //  @param   path - The file path to normalize.
        //  @return  A normalized version of the input path.
        // ---------------------------------------------------------------------
        static std::string Normalize(const std::string& path);

        // ---------------------------------------------------------------------
        //  Function: Join
        //  Purpose : Combines multiple path segments into one valid path.
        //  @param   parts - Vector of path segments.
        //  @return  Joined path string.
        // ---------------------------------------------------------------------
        static std::string Join(const std::vector<std::string>& parts);

        // ---------------------------------------------------------------------
        //  Function: GetFileName
        //  Purpose : Extracts the file name from a full path (without directories).
        //  @param   path - The input full file path.
        //  @return  File name portion of the path.
        // ---------------------------------------------------------------------
        static std::string GetFileName(const std::string& path);

        // ---------------------------------------------------------------------
        //  Function: GetExtension
        //  Purpose : Returns the file extension, if one exists.
        //  @param   path - The input path.
        //  @return  The file extension, including the '.' (e.g., ".txt").
        // ---------------------------------------------------------------------
        static std::string GetExtension(const std::string& path);

        // ---------------------------------------------------------------------
        //  Function: GetDirectory
        //  Purpose : Extracts the directory part of a full path.
        //  @param   path - The input path.
        //  @return  The directory path (without filename).
        // ---------------------------------------------------------------------
        static std::string GetDirectory(const std::string& path);

        // ---------------------------------------------------------------------
        //  Function: Exists
        //  Purpose : Checks if a given path exists on disk.
        //  @param   path - Path to check.
        //  @return  True if file or directory exists, otherwise false.
        // ---------------------------------------------------------------------
        static bool Exists(const std::string& path);
    };
}

