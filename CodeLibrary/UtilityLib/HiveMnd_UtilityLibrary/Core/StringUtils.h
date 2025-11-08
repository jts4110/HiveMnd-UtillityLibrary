#pragma once
#include <string>
#include <vector>
#include <algorithm>

/*
============================================================
File: StringUtils.h
Purpose: This header defines the StringUtils class which
provides basic string utility functions for the HiveMnd
Utility Library Core module.
============================================================
*/

namespace HiveMnd
{
    namespace Core
    {
        /*
        ============================================================
        Class: StringUtils
        Description:
            - A static utility class that holds helper functions
              for manipulating and processing std::string objects.
            - These methods simplify common string operations like
              converting case and splitting text into pieces.
        ============================================================
        */
        class StringUtils
        {
        public:
            /*
            ============================================================
            Function: ToUpper
            Parameters: const std::string& input
            Returns: std::string
            Description:
                Converts all characters in a string to uppercase.
                Useful when comparing text case-insensitively or when
                formatting strings for display in consistent casing.
            ============================================================
            */
            static std::string ToUpper(const std::string& input);

            /*
            ============================================================
            Function: ToLower
            Parameters: const std::string& input
            Returns: std::string
            Description:
                Converts all characters in a string to lowercase.
                Often used before text comparisons or parsing to
                ensure consistent case handling.
            ============================================================
            */
            static std::string ToLower(const std::string& input);

            /*
            ============================================================
            Function: Split
            Parameters: const std::string& input, char delimiter
            Returns: std::vector<std::string>
            Description:
                Splits a string into parts wherever the specified
                delimiter character appears. Each section of text
                between delimiters becomes an element in the returned
                vector. Useful for parsing comma-separated values,
                commands, or user input.
            ============================================================
            */
            static std::vector<std::string> Split(const std::string& input, char delimiter);
        };
    }
}

