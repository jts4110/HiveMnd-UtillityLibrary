#include "PasswordVaultUtils.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>

namespace HiveMnd::Security
{
    // -------------------------------------------------------------------------
    // Internal fallback encoder/decoder
    // -------------------------------------------------------------------------
    // These act as temporary substitutes for Base64 until Base64Utils is added.
    // They encode data as hexadecimal strings for easy verification.
    // -------------------------------------------------------------------------
    static std::string FallbackEncode(const std::vector<uint8_t>& data)
    {
        std::ostringstream oss;
        oss << std::hex << std::setfill('0');
        for (uint8_t b : data)
            oss << std::setw(2) << static_cast<int>(b);
        return oss.str();
    }

    static std::vector<uint8_t> FallbackDecode(const std::string& hex)
    {
        std::vector<uint8_t> result;
        result.reserve(hex.size() / 2);

        for (size_t i = 0; i + 1 < hex.size(); i += 2)
        {
            std::string byteStr = hex.substr(i, 2);
            uint8_t byte = static_cast<uint8_t>(std::stoi(byteStr, nullptr, 16));
            result.push_back(byte);
        }
        return result;
    }

    // -------------------------------------------------------------------------
    // Function: Load
    // Purpose : Reads and decodes vault data from file.
    // -------------------------------------------------------------------------
    bool PasswordVault::Load(const std::string& masterPassword)
    {
        std::ifstream file(mFilePath, std::ios::binary);
        if (!file)
        {
            std::cerr << "[Vault] Could not open vault file: " << mFilePath << "\n";
            return false;
        }

        std::ostringstream buffer;
        buffer << file.rdbuf();
        std::string encoded = buffer.str();

        std::vector<uint8_t> decoded = FallbackDecode(encoded);
        mData.assign(decoded.begin(), decoded.end());
        return true;
    }

    // -------------------------------------------------------------------------
    // Function: Save
    // Purpose : Encodes and writes vault data to file.
    // -------------------------------------------------------------------------
    bool PasswordVault::Save(const std::string& masterPassword) const
    {
        std::ofstream file(mFilePath, std::ios::binary);
        if (!file)
        {
            std::cerr << "[Vault] Could not write vault file: " << mFilePath << "\n";
            return false;
        }

        std::vector<uint8_t> bytes(mData.begin(), mData.end());
        std::string encoded = FallbackEncode(bytes);
        file << encoded;
        return true;
    }
}
