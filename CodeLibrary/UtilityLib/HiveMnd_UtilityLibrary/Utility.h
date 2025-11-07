#pragma once
#include <iostream>
#include <cstring>
#include <limits>
#include <vector>

// ============================================================================
//  Project : HiveMnd Utility Library
//  File    : Utility.h
//  Version : v1.2.0
//  Author  : Jonathan Smith | HiveMnd LLC
//  Created : November 2025
// ----------------------------------------------------------------------------
//  Description:
//  A comprehensive, reusable C++20 utility header containing helper methods for 
//  console interaction, numeric operations, string manipulation, validation,
//  file handling, and other general-purpose functions.
//
//  This library was developed as a professional evolution of earlier academic 
//  helper libraries, refined and expanded under the HiveMnd development suite.
//
// ----------------------------------------------------------------------------
//  Usage:
//  Include this file in your C++ project and call functions using the
//  HiveMnd::Utility namespace.
//
//      Example:
//          #include "Utility.h"
//          int main()
//          {
//              HiveMnd::Utility::PrintTimestamp();
//              return 0;
//          }
//
// ----------------------------------------------------------------------------
//  License & Usage Warning:
//  © 2025 HiveMnd LLC. All rights reserved.
//  Unauthorized use, modification, or distribution of this file, in whole or in 
//  part, without express written consent from HiveMnd LLC is strictly prohibited.
//  This file and its contents are proprietary intellectual property.
//
//  Redistribution for educational, private, or commercial use without 
//  authorization is not permitted.
//
// ----------------------------------------------------------------------------
//  Notes:
//  - Designed for cross-project reuse (academic & commercial).
//  - Uses only standard C++20 features and system-level calls.
//  - Safe pointer validation and clear commenting throughout.
// ============================================================================




namespace HiveMnd::Utility
{

    // -----------------------------------------------------------------------------
    //  Function: HiveMnd::Utility::IsInteger
    //  Purpose : Validates whether a given C-string represents a valid integer.
    //  @param  input - Pointer to a C-style string containing user input.
    //  @return bool  - True if the string contains a valid integer, otherwise false.
    //  Notes  : Handles negative numbers. Rejects empty or malformed strings.
    // -----------------------------------------------------------------------------

    static bool IsInteger(char* input)
    {

        if (input == nullptr || strlen(input) == 0)
        {

            return false;

        }

        int startIndex = 0;
        if (input[0] == '-')
        {

            if (strlen(input) == 1)
            {

                return false;

            }

            startIndex = 1;

        }

        for (int i = startIndex; input[i] != '\0'; ++i)
        {

            if (input[i] < '0' || input[i] > '9')
            {

                return false;

            }
        }

        return true;

    }

    // -----------------------------------------------------------------------------
    //  Function: HiveMnd::Utility::PrintIntegerBinary
    //  Purpose : Prints the full 32-bit binary representation of an integer.
    //  @param  num - Pointer to the integer to convert.
    //  @return void
    //  Notes  : Groups output into 8-bit segments for readability. Does not use bitset.
    // -----------------------------------------------------------------------------

    static void PrintIntegerBinary(int* num)
    {

        if (num == nullptr)
        {

            return;

        }

        unsigned int mask = 1u << 31;

        for (int i = 0; i < 32; ++i)
        {

            std::cout << ((*num & mask) ? '1' : '0');

            if ((i + 1) % 8 == 0)
            {

                std::cout << ' ';

            }

            mask >>= 1;

        }

        std::cout << std::endl;

    }

    // -----------------------------------------------------------------------------
    //  Function: HiveMnd::Utility::BubbleSort
    //  Purpose : Sorts an integer array in ascending order using the Bubble Sort algorithm.
    //  @param  array - Pointer to the integer array to sort.
    //  @param  size  - Number of elements in the array.
    //  @return void
    //  Notes  : Automatically exits early if no swaps occur during a pass.
    // -----------------------------------------------------------------------------

    static void BubbleSort(int* array, int size)
    {

        if (array == nullptr || size <= 1)
        {

            return;

        }

        bool swapped;

        for (int i = 0; i < size - 1; ++i)
        {

            swapped = false;

            for (int j = 0; j < size - i - 1; ++j)
            {

                if (array[j] > array[j + 1])
                {

                    int temp = array[j];
                    array[j] = array[j + 1];
                    array[j + 1] = temp;
                    swapped = true;

                }

            }

            if (!swapped)
            {
                break;

            }

        }

    }

    // -----------------------------------------------------------------------------
    //  Function: HiveMnd::Utility::ClearInputBuffer
    //  Purpose : Clears any leftover input data from the standard input buffer.
    //  @return void
    //  Notes  : Prevents unwanted characters from interfering with future input.
    // -----------------------------------------------------------------------------

    static void ClearInputBuffer()
    {

        std::cin.clear();

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    }

    // -----------------------------------------------------------------------------
    //  Function: HiveMnd::Utility::ClearConsoleWindow
    //  Purpose : Clears the terminal window for a cleaner output view.
    //  @return void
    //  Notes  : Uses system("cls"); This function is Windows-only.
    // -----------------------------------------------------------------------------


    static void ClearConsoleWindow()
    {

        system("cls");

    }

    // -----------------------------------------------------------------------------
    //  Function: HiveMnd::Utility::PauseConsoleWindow
    //  Purpose : Pauses program execution until the user presses Enter.
    //  @return void
    //  Notes  : Ideal for debugging or preventing console auto-close.
    // -----------------------------------------------------------------------------

    static void PauseConsoleWindow()
    {

        std::cout << "Press Enter to continue...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    }

    // -----------------------------------------------------------------------------
    //  Function: HiveMnd::Utility::IsPositiveInteger
    //  Purpose : Validates that a given string represents a positive integer (> 0).
    //  @param  input - Pointer to the C-string to validate.
    //  @return bool  - True if the string is a valid positive integer, otherwise false.
    // -----------------------------------------------------------------------------

    static bool isPositiveInteger(char* input)
    {

        if (input == nullptr || strlen(input) == 0)
        {

            return false;

        }

        if (input[0] == '-')
        {

            return false;

        }

        for (int i = 0; input[i] != '\0'; ++i)
        {

            if (input[i] < '0' || input[i] > '9')
            {

                return false;

            }

        }

        int value = atoi(input);

        return (value > 0);

    }

    // -----------------------------------------------------------------------------
    //  Function: HiveMnd::Utility::RandomNumberGenerator
    //  Purpose : Generates a random integer between a specified minimum and maximum.
    //  @param  min - Minimum possible value (inclusive).
    //  @param  max - Maximum possible value (inclusive).
    //  @return int - Randomly generated number within range.
    //  Notes  : Seeds the random number generator once per program run.
    // -----------------------------------------------------------------------------

    static int RandomNumberGenerator(int min = 1, int max = 100)
    {

        if (min > max)
        {

            int temp = min;
            min = max;
            max = temp;

        }

        static bool randomInitialized = false;

        if (!randomInitialized)
        {

            srand(static_cast<unsigned int>(time(nullptr)));
            randomInitialized = true;

        }

        int range = (max - min) + 1;
        int randomNum = rand() % range + min;
        return randomNum;

    }

    // -----------------------------------------------------------------------------
    //  Function: HiveMnd::Utility::PrintIntegerHex
    //  Purpose : Displays an integer in hexadecimal format.
    //  @param  num - Pointer to the integer to convert.
    //  @return void
    //  Notes  : Shows both decimal and hexadecimal formats side-by-side.
    // -----------------------------------------------------------------------------

    static void PrintIntegerHex(int* num)
    {

        if (num == nullptr)
        {

            std::cout << "Invalid Pointer.\n";
            return;

        }

        std::cout << "Decimal: " << *num << " | Hexidecimal: 0x" << std::uppercase << std::hex << *num << std::dec << std::nouppercase << std::endl;

    }

    // -----------------------------------------------------------------------------
    //  Function: HiveMnd::Utility::PrintIntegerOct
    //  Purpose : Displays an integer in octal format.
    //  @param  num - Pointer to the integer to convert.
    //  @return void
    //  Notes  : Displays both decimal and octal versions for clarity.
    // -----------------------------------------------------------------------------

    static void PrintIntegerOct(int* num)
    {

        if (num == nullptr)
        {

            std::cout << "Invalid Pointer.\n";
            return;

        }

        std::cout << "Decimal: " << *num << " | Octal: 0" << std::oct << *num << std::dec << std::endl;

    }

    // -----------------------------------------------------------------------------
    //  Function: HiveMnd::Utility::UserInputBool
    //  Purpose : Prompts the user to input a boolean value (true/false).
    //  @param  out - Pointer to a bool where the result will be stored.
    //  @return void
    //  Notes  : Accepts only "true" or "false" (case-insensitive). Re-prompts until valid.
    // -----------------------------------------------------------------------------

    static void UserInputBool(bool* out)
    {

        if (out == nullptr)
        {

            return;

        }

        char buffer[32];

        while (true)
        {

            std::cout << "Enter True or False: ";
            std::cin.getline(buffer, sizeof(buffer));

            if (std::cin.fail())
            {

                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;

            }

            for (int i = 0; buffer[i] != '\0'; ++i)
            {

                buffer[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(buffer[i])));

            }

            if (std::strcmp(buffer, "true") == 0)
            {

                *out = true;
                break;

            }

            else if (std::strcmp(buffer, "false") == 0)
            {

                *out = false;
                break;

            }

            else
            {

                std::cout << "Invalid input. Please Enter True or False.\n";

            }

        }

    }

    // -----------------------------------------------------------------------------
    //  Function: HiveMnd::Utility::IntegerSwap
    //  Purpose : Swaps the values of two integers using pointers.
    //  @param  a - Pointer to the first integer.
    //  @param  b - Pointer to the second integer.
    //  @return void
    //  Notes  : Performs the swap in-place using a temporary variable.
    // -----------------------------------------------------------------------------

    static void IntegerSwap(int* a, int* b)
    {

        if (a == nullptr || b == nullptr)
        {

            return;

        }

        int temp = *a;
        *a = *b;
        *b = temp;

    }

    // -----------------------------------------------------------------------------
    //  Function: HiveMnd::Utility::IsEven
    //  Purpose : Determines whether an integer is even.
    //  @param  num - Pointer to the integer to check.
    //  @return bool - True if even, false otherwise.
    // -----------------------------------------------------------------------------

    static bool IsEven(int* num)
    {

        if (num == nullptr)
        {

            return false;

        }

        return (*num % 2 == 0);

    }

    // -----------------------------------------------------------------------------
    //  Function: HiveMnd::Utility::GetMaxValue
    //  Purpose : Returns the greater of two integers.
    //  @param  num1 - Pointer to the first integer.
    //  @param  num2 - Pointer to the second integer.
    //  @return int - The larger of the two integers (0 if both null).
    // -----------------------------------------------------------------------------

    static int GetMaxValue(int* num1, int* num2)
    {

        if (num1 == nullptr || num2 == nullptr)
        {

            return 0;

        }

        else if (num1 == nullptr)
        {

            return *num2;

        }

        else if (num2 == nullptr)
        {

            return *num1;

        }

        return (*num1 > *num2) ? *num1 : *num2;

    }

    // -----------------------------------------------------------------------------
    //  Function: HiveMnd::Utility::GetMinValue
    //  Purpose : Returns the smaller of two integers.
    //  @param  num1 - Pointer to the first integer.
    //  @param  num2 - Pointer to the second integer.
    //  @return int - The smaller of the two integers (0 if both null).
    // -----------------------------------------------------------------------------

    static int GetMinValue(int* num1, int* num2)
    {

        if (num1 == nullptr && num2 == nullptr)
        {

            return 0;

        }

        else if (num1 == nullptr)
        {

            return *num2;

        }

        else if (num2 == nullptr)
        {

            return *num1;

        }

        return (*num1 < *num2) ? *num1 : *num2;

    }

    // -----------------------------------------------------------------------------
    //  Function: HiveMnd::Utility::ClampInteger
    //  Purpose : Clamps an integer to a specified range (min to max).
    //  @param  value - Pointer to the integer to clamp.
    //  @param  min   - Minimum allowed value.
    //  @param  max   - Maximum allowed value.
    //  @return void
    // -----------------------------------------------------------------------------

    static void ClampInteger(int* value, int min, int max)
    {

        if (value == nullptr)
        {

            return;

        }

        if (*value < min)
        {

            *value = min;

        }

        else if (*value > max)
        {

            *value = max;

        }

    }

    // -----------------------------------------------------------------------------
    //  Function: HiveMnd::Utility::IsPrime
    //  Purpose : Checks whether a number is prime.
    //  @param  num - Integer to evaluate.
    //  @return bool - True if prime, false otherwise.
    //  Notes  : Uses optimized 6k ± 1 rule for efficiency.
    // -----------------------------------------------------------------------------

    static bool IsPrime(int num)
    {

        if (num <= 1)
        {

            return false;

        }

        if (num <= 3)
        {

            return true;

        }

        if (num % 2 == 0 || num % 3 == 0)
        {

            return false;

        }

        for (int i = 5; i * i <= num; i += 6)
        {

            if (num % i == 0 || num % (i + 2) == 0)
            {

                return false;

            }

        }

        return true;

    }

    // -----------------------------------------------------------------------------
    //  Function: HiveMnd::Utility::UserInputChar
    //  Purpose : Prompts user to input a single character.
    //  @param  input - Pointer to character buffer (must hold at least 2 chars).
    //  @return void
    //  Notes  : Re-prompts until only one character is entered.
    // -----------------------------------------------------------------------------

    static void UserInputChar(char* input)
    {

        if (input == nullptr)
        {

            return;

        }

        while (true)
        {

            std::cout << "Enter a single character: ";
            std::cin.getline(input, 50);

            if (strlen(input) == 1)
            {

                break;

            }

            else
            {

                std::cout << "Invalid input. Please enter only ONE character: ";

            }

        }

        std::cin.clear();

    }

    // -----------------------------------------------------------------------------
    //  Function: HiveMnd::Utility::ValidateRange
    //  Purpose : Verifies that an integer lies within a specified range.
    //  @param  value - Integer value to check.
    //  @param  min   - Minimum acceptable value.
    //  @param  max   - Maximum acceptable value.
    //  @return bool - True if within range, false otherwise.
    // -----------------------------------------------------------------------------

    static bool ValidateRange(int value, int min, int max)
    {

        if (value < min || value > max)
        {

            return false;

        }

        return true;

    }

    // -----------------------------------------------------------------------------
    //  Function: HiveMnd::Utility::SafeDivide
    //  Purpose : Performs integer division safely (avoids divide-by-zero).
    //  @param  numerator   - The number to be divided.
    //  @param  denominator - The number to divide by.
    //  @return int - The division result, or 0 if denominator is zero.
    //  Notes  : Prints a warning if division by zero is attempted.
    // -----------------------------------------------------------------------------

    static int SafeDivide(int numerator, int demoninator)
    {

        if (demoninator == 0)
        {

            std::cout << "Warning: Cannot divide by 0. Result set to 0\n";
            return 0;

        }

        return numerator / demoninator;

    }

    // -----------------------------------------------------------------------------
    //  Function: HiveMnd::Utility::VectorSum
    //  Purpose : Calculates the total sum of all elements in a vector.
    //  @param  vec - Pointer to a std::vector<int> containing the numbers.
    //  @return int - The sum of all vector elements (0 if vector is null).
    // -----------------------------------------------------------------------------

    static int VectorSum(std::vector<int>* vec)
    {

        if (vec == nullptr)
        {

            std::cout << "Vector pointer is null.\n";
            return 0;

        }

        int sum = 0;

        for (int i = 0; i < vec->size(); ++i)
        {

            sum += (*vec)[i];

        }

        return sum;

    }

    // -----------------------------------------------------------------------------
    //  Function: HiveMnd::Utility::UserInputString
    //  Purpose : Prompts the user to enter a string of text.
    //  @param  buffer - Pointer to a character array where the string will be stored.
    //  @param  bufferSize - Maximum number of characters to read (including null terminator).
    //  @return void
    //  Notes  : Prevents buffer overflow and clears input on invalid entries.
    // -----------------------------------------------------------------------------
    static void UserInputString(char* buffer, int bufferSize)
    {
        if (buffer == nullptr || bufferSize <= 1)
        {
            std::cout << "Invalid buffer provided.\n";
            return;
        }

        std::cout << "Enter text: ";
        std::cin.getline(buffer, bufferSize);

        if (std::cin.fail())
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Input exceeded maximum length (" << bufferSize - 1 << " chars). Truncated.\n";
        }
    }

    // -----------------------------------------------------------------------------
    //  Function: HiveMnd::Utility::UserInputInteger
    //  Purpose : Prompts the user to enter a valid integer value.
    //  @param  outValue - Pointer to an integer variable where input will be stored.
    //  @return void
    //  Notes  : Automatically re-prompts until a valid integer is entered.
    // -----------------------------------------------------------------------------
    static void UserInputInteger(int* outValue)
    {
        if (outValue == nullptr)
        {
            std::cout << "Invalid integer pointer.\n";
            return;
        }

        while (true)
        {
            std::cout << "Enter an integer: ";
            std::cin >> *outValue;

            if (std::cin.fail())
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Invalid input. Please enter a valid integer.\n";
                continue;
            }

            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            break;
        }
    }

    // -----------------------------------------------------------------------------
    //  Function: HiveMnd::Utility::Write
    //  Purpose : Prints text to the console without a newline at the end.
    //  @param  text - The text to print.
    //  @return void
    // -----------------------------------------------------------------------------
    static void Write(const char* text)
    {
        if (text == nullptr)
            return;

        std::cout << text;
    }

    // -----------------------------------------------------------------------------
    //  Function: HiveMnd::Utility::WriteLine
    //  Purpose : Prints text to the console followed by a newline.
    //  @param  text - The text to print.
    //  @return void
    // -----------------------------------------------------------------------------
    static void WriteLine(const char* text)
    {
        if (text == nullptr)
            return;

        std::cout << text << std::endl;
    }

    // -----------------------------------------------------------------------------
    //  Function: HiveMnd::Utility::ChangeBackgroundColor
    //  Purpose : Changes the console background color using Windows system command.
    //  @param  colorCode - Integer representing the desired color (0–15 typical).
    //  @return void
    //  Notes  : Works on Windows. See color codes: 
    //           0=Black, 1=Blue, 2=Green, 4=Red, 7=White, etc.
    // -----------------------------------------------------------------------------
    static void ChangeBackgroundColor(int colorCode)
    {
        if (colorCode < 0 || colorCode > 15)
        {
            std::cout << "Invalid color code. Must be between 0 and 15.\n";
            return;
        }

        // Format command string: "color XY" where X=background, Y=text
        char command[10];
        sprintf_s(command, "color %X", colorCode);
        system(command);
    }

    // -----------------------------------------------------------------------------
    //  Function: HiveMnd::Utility::ChangeForegroundColor
    //  Purpose : Changes the console text (foreground) color using Windows system command.
    //  @param  colorCode - Integer representing the desired color (0–15 typical).
    //  @return void
    //  Notes  : Works on Windows. Common codes: 
    //           0=Black, 7=White, 9=Light Blue, 10=Light Green, 12=Light Red.
    // -----------------------------------------------------------------------------
    static void ChangeForegroundColor(int colorCode)
    {
        if (colorCode < 0 || colorCode > 15)
        {
            std::cout << "Invalid color code. Must be between 0 and 15.\n";
            return;
        }

        // Combine with existing background (optional future upgrade)
        char command[10];
        sprintf_s(command, "color %X", colorCode);
        system(command);
    }

    // -----------------------------------------------------------------------------
//  Function: HiveMnd::Utility::DrawLine
//  Purpose : Prints a horizontal divider line for cleaner console layouts.
//  @param  c       - Character used to draw the line (default '-')
//  @param  length  - Number of characters in the line (default 40)
//  @return void
// -----------------------------------------------------------------------------
    static void DrawLine(char c = '-', int length = 40)
    {
        for (int i = 0; i < length; ++i)
        {
            std::cout << c;
        }
        std::cout << std::endl;
    }

    // -----------------------------------------------------------------------------
    //  Function: HiveMnd::Utility::CenterText
    //  Purpose : Centers a string within a specified console width.
    //  @param  text   - Pointer to the string to center
    //  @param  width  - Console width to center within (default 80)
    //  @return void
    // -----------------------------------------------------------------------------
    static void CenterText(const char* text, int width = 80)
    {
        if (text == nullptr)
            return;

        int len = static_cast<int>(strlen(text));
        if (len >= width)
        {
            std::cout << text << std::endl;
            return;
        }

        int padding = (width - len) / 2;
        for (int i = 0; i < padding; ++i)
            std::cout << ' ';

        std::cout << text << std::endl;
    }

    // -----------------------------------------------------------------------------
    //  Function: HiveMnd::Utility::PrintTimestamp
    //  Purpose : Displays the current system date and time in YYYY-MM-DD HH:MM:SS format.
    //  @return void
    // -----------------------------------------------------------------------------
    static void PrintTimestamp()
    {
        time_t now = time(nullptr);
        tm localTime{};
#ifdef _WIN32
        localtime_s(&localTime, &now);
#else
        localtime_r(&now, &localTime);
#endif
        char buffer[64];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &localTime);
        std::cout << "Timestamp: " << buffer << std::endl;
    }

    // -----------------------------------------------------------------------------
    //  Function: HiveMnd::Utility::SetConsoleTitle
    //  Purpose : Sets the console window title text (Windows-only).
    //  @param  title - String representing the desired console title
    //  @return void
    //  Notes  : Does nothing on non-Windows platforms.
    // -----------------------------------------------------------------------------
    static void SetConsoleTitle(const char* title)
    {
#ifdef _WIN32
        if (title != nullptr)
        {
            SetConsoleTitleA(title);
        }
#else
        (void)title;
        std::cout << "(Console title setting is Windows-only)\n";
#endif
    }

    // -----------------------------------------------------------------------------
    //  Function: HiveMnd::Utility::ClampFloat
    //  Purpose : Keeps a float value within a specified range.
    //  @param  value - Pointer to float to clamp
    //  @param  min   - Minimum allowable value
    //  @param  max   - Maximum allowable value
    //  @return void
    // -----------------------------------------------------------------------------
    static void ClampFloat(float* value, float min, float max)
    {
        if (value == nullptr)
            return;

        if (*value < min)
            *value = min;
        else if (*value > max)
            *value = max;
    }

    // -----------------------------------------------------------------------------
    //  Function: HiveMnd::Utility::Lerp
    //  Purpose : Performs linear interpolation between two float values.
    //  @param  a - Starting value
    //  @param  b - Ending value
    //  @param  t - Interpolation amount (0–1 range)
    //  @return float - Interpolated value
    // -----------------------------------------------------------------------------
    static float Lerp(float a, float b, float t)
    {
        if (t < 0.0f)
            t = 0.0f;
        if (t > 1.0f)
            t = 1.0f;
        return a + (b - a) * t;
    }

    // -----------------------------------------------------------------------------
    //  Function: HiveMnd::Utility::MapRange
    //  Purpose : Maps a value from one numeric range to another.
    //  @param  value  - The value to map
    //  @param  inMin  - Input range minimum
    //  @param  inMax  - Input range maximum
    //  @param  outMin - Output range minimum
    //  @param  outMax - Output range maximum
    //  @return float  - Mapped value in new range
    // -----------------------------------------------------------------------------
    static float MapRange(float value, float inMin, float inMax, float outMin, float outMax)
    {
        if (inMax - inMin == 0.0f)
            return outMin;

        return (value - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
    }

    // -----------------------------------------------------------------------------
    //  Function: HiveMnd::Utility::IsPowerOfTwo
    //  Purpose : Determines if a number is a power of two.
    //  @param  num - Integer to check
    //  @return bool - True if power of two, false otherwise
    // -----------------------------------------------------------------------------
    static bool IsPowerOfTwo(int num)
    {
        return (num > 0) && ((num & (num - 1)) == 0);
    }

    // -----------------------------------------------------------------------------
    //  Function: HiveMnd::Utility::RandomFloat
    //  Purpose : Generates a random floating-point number within the specified range.
    //  @param  min - Minimum range value
    //  @param  max - Maximum range value
    //  @return float - Random number between min and max
    // -----------------------------------------------------------------------------
    static float RandomFloat(float min, float max)
    {
        if (min > max)
        {
            float temp = min;
            min = max;
            max = temp;
        }

        static bool randomInitialized = false;
        if (!randomInitialized)
        {
            srand(static_cast<unsigned int>(time(nullptr)));
            randomInitialized = true;
        }

        float scale = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        return min + scale * (max - min);
    }

    // -----------------------------------------------------------------------------
    //  Function: HiveMnd::Utility::ToUpper
    //  Purpose : Converts all lowercase letters in a string to uppercase.
    //  @param  str - Pointer to character array to modify
    //  @return void
    // -----------------------------------------------------------------------------
    static void ToUpper(char* str)
    {
        if (str == nullptr)
            return;

        for (int i = 0; str[i] != '\0'; ++i)
        {
            str[i] = static_cast<char>(toupper(static_cast<unsigned char>(str[i])));
        }
    }

    // -----------------------------------------------------------------------------
    //  Function: HiveMnd::Utility::ToLower
    //  Purpose : Converts all uppercase letters in a string to lowercase.
    //  @param  str - Pointer to character array to modify
    //  @return void
    // -----------------------------------------------------------------------------
    static void ToLower(char* str)
    {
        if (str == nullptr)
            return;

        for (int i = 0; str[i] != '\0'; ++i)
        {
            str[i] = static_cast<char>(tolower(static_cast<unsigned char>(str[i])));
        }
    }

    // -----------------------------------------------------------------------------
    //  Function: HiveMnd::Utility::StringEqualsIgnoreCase
    //  Purpose : Compares two strings without case sensitivity.
    //  @param  a - First string
    //  @param  b - Second string
    //  @return bool - True if equal (ignoring case), false otherwise
    // -----------------------------------------------------------------------------
    static bool StringEqualsIgnoreCase(const char* a, const char* b)
    {
        if (a == nullptr || b == nullptr)
            return false;

        while (*a && *b)
        {
            if (tolower(static_cast<unsigned char>(*a)) !=
                tolower(static_cast<unsigned char>(*b)))
                return false;
            ++a;
            ++b;
        }
        return *a == *b;
    }

    // -----------------------------------------------------------------------------
    //  Function: HiveMnd::Utility::TrimWhitespace
    //  Purpose : Removes leading and trailing whitespace from a string.
    //  @param  str - Pointer to character array to modify
    //  @return void
    // -----------------------------------------------------------------------------
    static void TrimWhitespace(char* str)
    {
        if (str == nullptr)
            return;

        char* start = str;
        while (isspace(static_cast<unsigned char>(*start)))
            ++start;

        if (*start == '\0')
        {
            str[0] = '\0';
            return;
        }

        char* end = start + strlen(start) - 1;
        while (end > start && isspace(static_cast<unsigned char>(*end)))
            --end;

        *(end + 1) = '\0';
        memmove(str, start, end - start + 2);
    }

    // -----------------------------------------------------------------------------
    //  Function: HiveMnd::Utility::FileExists
    //  Purpose : Checks if a file exists at a given file path.
    //  @param  path - Pointer to the file path
    //  @return bool - True if file exists, false otherwise
    // -----------------------------------------------------------------------------
    static bool FileExists(const char* path)
    {
        if (path == nullptr)
            return false;

        FILE* file = nullptr;
#ifdef _WIN32
        fopen_s(&file, path, "r");
#else
        file = fopen(path, "r");
#endif
        if (file)
        {
            fclose(file);
            return true;
        }
        return false;
    }

    // -----------------------------------------------------------------------------
    //  Function: HiveMnd::Utility::ReadTextFile
    //  Purpose : Reads and prints all text from a given file path.
    //  @param  path - Pointer to file path string
    //  @return void
    // -----------------------------------------------------------------------------
    static void ReadTextFile(const char* path)
    {
        if (!FileExists(path))
        {
            std::cout << "File not found: " << path << std::endl;
            return;
        }

        FILE* file = nullptr;
        #ifdef _WIN32
            fopen_s(&file, path, "r");
        #else
            file = fopen(path, "r");
        #endif

        if (!file)
        {
            std::cout << "Unable to open file: " << path << std::endl;
            return;
        }

        char buffer[256];
        while (fgets(buffer, sizeof(buffer), file))
        {
            std::cout << buffer;
        }

        fclose(file);
    }


}

// ============================================================================
//  End of HiveMnd::Utility Library
//  © HiveMnd LLC — Jonathan Smith
//  Reuse or distribution without written consent is strictly prohibited.
// ============================================================================
