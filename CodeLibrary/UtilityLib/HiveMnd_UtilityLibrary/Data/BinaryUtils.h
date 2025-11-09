#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <iostream>

namespace HiveMnd::Data
{
    // =========================================================================
    // CLASS : BinaryUtils
    // PURPOSE: Provides binary file operations and basic encryption features.
    // NOTES  : Designed for performance and portability. The XOR encryption
    //          here is lightweight — good for obfuscation, not high security.
    // =========================================================================
    class BinaryUtils
    {
    public:
        // ---------------------------------------------------------------------
        // Function: WriteBinary
        // Purpose : Writes a block of binary data to a file.
        // ---------------------------------------------------------------------
        static bool WriteBinary(const std::string& filePath, const void* data, size_t size)
        {
            std::ofstream file(filePath, std::ios::out | std::ios::binary);
            if (!file.is_open())
            {
                std::cerr << "BinaryUtils: Failed to open file for writing: " << filePath << std::endl;
                return false;
            }

            file.write(reinterpret_cast<const char*>(data), size);
            if (!file.good())
            {
                std::cerr << "BinaryUtils: Write operation failed for file: " << filePath << std::endl;
                return false;
            }

            file.close();
            return true;
        }

        // ---------------------------------------------------------------------
        // Function: ReadBinary
        // Purpose : Reads binary data from a file into a vector buffer.
        // ---------------------------------------------------------------------
        static bool ReadBinary(const std::string& filePath, std::vector<char>& outBuffer)
        {
            std::ifstream file(filePath, std::ios::in | std::ios::binary);
            if (!file.is_open())
            {
                std::cerr << "BinaryUtils: Failed to open file: " << filePath << std::endl;
                return false;
            }

            file.seekg(0, std::ios::end);
            std::streampos fileSize = file.tellg();
            file.seekg(0, std::ios::beg);

            outBuffer.resize(static_cast<size_t>(fileSize));
            file.read(outBuffer.data(), fileSize);

            if (!file.good() && !file.eof())
            {
                std::cerr << "BinaryUtils: Read operation failed for file: " << filePath << std::endl;
                return false;
            }

            file.close();
            return true;
        }

        // ---------------------------------------------------------------------
        // Function: AppendBinary
        // Purpose : Appends binary data to the end of a file.
        // ---------------------------------------------------------------------
        static bool AppendBinary(const std::string& filePath, const void* data, size_t size)
        {
            std::ofstream file(filePath, std::ios::app | std::ios::binary);
            if (!file.is_open())
            {
                std::cerr << "BinaryUtils: Failed to open file for appending: " << filePath << std::endl;
                return false;
            }

            file.write(reinterpret_cast<const char*>(data), size);
            file.close();
            return true;
        }

        // ---------------------------------------------------------------------
        // Function: GetFileSize
        // Purpose : Returns the total number of bytes in a file.
        // ---------------------------------------------------------------------
        static size_t GetFileSize(const std::string& filePath)
        {
            std::ifstream file(filePath, std::ios::binary | std::ios::ate);
            if (!file.is_open())
                return 0;

            std::streampos size = file.tellg();
            file.close();
            return static_cast<size_t>(size);
        }

        // ---------------------------------------------------------------------
        // Function: PrintHexDump
        // Purpose : Prints a readable hexadecimal representation of data.
        // ---------------------------------------------------------------------
        static void PrintHexDump(const std::vector<char>& data)
        {
            std::cout << "Hex Dump (" << data.size() << " bytes):" << std::endl;

            for (size_t i = 0; i < data.size(); ++i)
            {
                printf("%02X ", static_cast<unsigned char>(data[i]));
                if ((i + 1) % 16 == 0)
                    std::cout << std::endl;
            }

            std::cout << std::endl;
        }

        // ---------------------------------------------------------------------
        // Function: XOREncryptDecrypt
        // Purpose : Applies simple XOR encryption/decryption to data.
        // Notes   : Using the same key on encrypted data will decrypt it.
        // Params  : data - Vector of bytes to modify.
        //           key  - Text key used to XOR the bytes.
        // ---------------------------------------------------------------------
        static void XOREncryptDecrypt(std::vector<char>& data, const std::string& key)
        {
            if (key.empty())
                return;

            size_t keyLen = key.length();
            for (size_t i = 0; i < data.size(); ++i)
            {
                // XOR each byte with corresponding key character (looping key)
                data[i] ^= key[i % keyLen];
            }
        }

        // ---------------------------------------------------------------------
        // Function: WriteEncrypted
        // Purpose : Encrypts data with XOR and writes it to disk.
        // ---------------------------------------------------------------------
        static bool WriteEncrypted(const std::string& filePath, const void* data, size_t size, const std::string& key)
        {
            std::vector<char> buffer(reinterpret_cast<const char*>(data), reinterpret_cast<const char*>(data) + size);
            XOREncryptDecrypt(buffer, key);
            return WriteBinary(filePath, buffer.data(), buffer.size());
        }

        // ---------------------------------------------------------------------
        // Function: ReadEncrypted
        // Purpose : Reads encrypted data, decrypts it, and stores the result.
        // ---------------------------------------------------------------------
        static bool ReadEncrypted(const std::string& filePath, std::vector<char>& outBuffer, const std::string& key)
        {
            if (!ReadBinary(filePath, outBuffer))
                return false;

            XOREncryptDecrypt(outBuffer, key);
            return true;
        }
    };
}
