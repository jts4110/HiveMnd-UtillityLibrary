#pragma once
#include <string>
#include <vector>

namespace HiveMnd::Core
{
    // -----------------------------------------------------------------------------
    //  Class: FileUtils
    //  Purpose : Provides file management helpers such as reading, writing, checking
    //            existence, and retrieving file metadata.
    //  Notes   : All functions are static and cross-platform where possible.
    // -----------------------------------------------------------------------------
    class FileUtils
    {
    public:
        // -----------------------------------------------------------------------------
        //  Function: FileExists
        //  Purpose : Checks whether a file exists on disk.
        //  @param  path - Path to the file to check.
        //  @return True if the file exists, otherwise false.
        // -----------------------------------------------------------------------------
        static bool FileExists(const std::string& path);

        // -----------------------------------------------------------------------------
        //  Function: ReadAllText
        //  Purpose : Reads the entire contents of a text file into a string.
        //  @param  path - Path to the file.
        //  @return A string containing the file contents, or empty if failed.
        // -----------------------------------------------------------------------------
        static std::string ReadAllText(const std::string& path);

        // -----------------------------------------------------------------------------
        //  Function: WriteAllText
        //  Purpose : Writes a string to a text file, overwriting existing content.
        //  @param  path - File path.
        //  @param  content - The text content to write.
        //  @return True if successful, false otherwise.
        // -----------------------------------------------------------------------------
        static bool WriteAllText(const std::string& path, const std::string& content);

        // -----------------------------------------------------------------------------
        //  Function: ReadAllLines
        //  Purpose : Reads a text file line by line into a vector of strings.
        //  @param  path - Path to the text file.
        //  @return Vector containing each line of text, or empty if failed.
        // -----------------------------------------------------------------------------
        static std::vector<std::string> ReadAllLines(const std::string& path);

        // -----------------------------------------------------------------------------
        //  Function: GetFileSize
        //  Purpose : Retrieves the size of a file in bytes.
        //  @param  path - Path to the file.
        //  @return File size in bytes, or 0 if not found or inaccessible.
        // -----------------------------------------------------------------------------
        static size_t GetFileSize(const std::string& path);

        // -----------------------------------------------------------------------------
        //  Function: DeleteFile
        //  Purpose : Deletes a file from disk.
        //  @param  path - Path to the file to delete.
        //  @return True if successfully deleted, false otherwise.
        // -----------------------------------------------------------------------------
        static bool DeleteFile(const std::string& path);
    };
}

