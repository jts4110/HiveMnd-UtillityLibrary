#include "StringUtils.h"
#include <sstream>

/*
============================================================
File: StringUtils.cpp
Purpose: Implements the methods defined in StringUtils.h.
============================================================
*/

namespace HiveMnd
{
    namespace Core
    {
        // ============================================================
        // Converts all characters in the input string to uppercase.
        // Uses std::transform to apply toupper() to each character.
        // ============================================================
        std::string StringUtils::ToUpper(const std::string& input)
        {
            std::string result = input;
            std::transform(result.begin(), result.end(), result.begin(),
                [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
            return result;
        }

        // ============================================================
        // Converts all characters in the input string to lowercase.
        // Similar logic as ToUpper, but uses std::tolower().
        // ============================================================
        std::string StringUtils::ToLower(const std::string& input)
        {
            std::string result = input;
            std::transform(result.begin(), result.end(), result.begin(),
                [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
            return result;
        }

        // ============================================================
        // Splits a string based on a single-character delimiter.
        // Uses stringstream to read parts separated by the delimiter
        // and stores each one in a std::vector.
        // Example: "A,B,C" → ["A","B","C"]
        // ============================================================
        std::vector<std::string> StringUtils::Split(const std::string& input, char delimiter)
        {
            std::vector<std::string> tokens;
            std::stringstream ss(input);
            std::string item;

            while (std::getline(ss, item, delimiter))
            {
                tokens.push_back(item);
            }

            return tokens;
        }
    }
}
