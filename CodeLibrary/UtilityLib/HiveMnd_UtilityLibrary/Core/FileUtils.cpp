#define _CRT_SECURE_NO_WARNINGS
#include "FileUtils.h"
#include <fstream>          // for reading and writing files
#include <filesystem>       // for checking file existence and size

namespace HiveMnd::Core
{
    // -----------------------------------------------------------------------------
    //  Function: FileExists
    //  Purpose : Checks if a file exists at the given path using std::filesystem.
    //  Returns : true if the file exists, false otherwise.
    // -----------------------------------------------------------------------------
    bool FileUtils::FileExists(const std::string& path)
    {
        // std::filesystem::exists returns true if file or directory exists
        return std::filesystem::exists(path);
    }

    // -----------------------------------------------------------------------------
    //  Function: ReadAllText
    //  Purpose : Reads the entire contents of a text file into a single string.
    //  Notes   : Returns an empty string if the file cannot be opened.
    // -----------------------------------------------------------------------------
    std::string FileUtils::ReadAllText(const std::string& path)
    {
        std::ifstream file(path); // open file for reading
        if (!file.is_open())
            return ""; // if file failed to open, return empty string

        // use iterators to read entire file content into a string
        std::string content((std::istreambuf_iterator<char>(file)),
            std::istreambuf_iterator<char>());
        return content;
    }

    // -----------------------------------------------------------------------------
    //  Function: WriteAllText
    //  Purpose : Writes the provided string content to a text file.
    //  Notes   : Overwrites existing contents. Returns true on success.
    // -----------------------------------------------------------------------------
    bool FileUtils::WriteAllText(const std::string& path, const std::string& content)
    {
        std::ofstream file(path); // open file for writing (overwrites existing)
        if (!file.is_open())
            return false; // failed to open file for writing

        file << content;  // write string content to file
        return true;      // success
    }

    // -----------------------------------------------------------------------------
    //  Function: ReadAllLines
    //  Purpose : Reads a text file line by line and returns all lines in a vector.
    //  Notes   : If file cannot be opened, returns an empty vector.
    // -----------------------------------------------------------------------------
    std::vector<std::string> FileUtils::ReadAllLines(const std::string& path)
    {
        std::ifstream file(path);      // open file for reading
        std::vector<std::string> lines; // to hold each line of text
        std::string line;

        if (!file.is_open())
            return lines; // return empty if failed to open

        // read file line by line
        while (std::getline(file, line))
        {
            lines.push_back(line); // add each line to vector
        }

        return lines; // return all lines read from file
    }

    // -----------------------------------------------------------------------------
    //  Function: GetFileSize
    //  Purpose : Returns the size of the file in bytes.
    //  Notes   : Returns 0 if file does not exist or cannot be accessed.
    // -----------------------------------------------------------------------------
    size_t FileUtils::GetFileSize(const std::string& path)
    {
        if (!std::filesystem::exists(path))
            return 0; // file not found, return 0

        // use std::filesystem to get file size
        return std::filesystem::file_size(path);
    }

    // -----------------------------------------------------------------------------
    //  Function: DeleteFile
    //  Purpose : Deletes the file at the specified path.
    //  Notes   : Returns true if file was successfully deleted.
    // -----------------------------------------------------------------------------
    bool FileUtils::DeleteFile(const std::string& path)
    {
        if (std::filesystem::exists(path))
        {
            // remove() returns true if file deleted successfully
            return std::filesystem::remove(path);
        }

        // file did not exist to begin with
        return false;
    }
}
