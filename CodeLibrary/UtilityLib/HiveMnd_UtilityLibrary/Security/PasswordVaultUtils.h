#pragma once
#include <string>
#include <vector>

namespace HiveMnd::Security
{
    // -------------------------------------------------------------------------
    // PasswordVault
    // -------------------------------------------------------------------------
    // Simple secure vault interface for storing and loading encrypted data.
    // Currently uses a lightweight fallback encoder (hex) until Base64Utils
    // is re-enabled.  Handles file I/O and string serialization.
    // -------------------------------------------------------------------------
    class PasswordVault
    {
    private:
        std::string mFilePath;
        std::string mData;

    public:
        PasswordVault(const std::string& filePath = "HiveMndVault.dat")
            : mFilePath(filePath) {
        }

        // Load existing vault from disk (auto-decrypts)
        bool Load(const std::string& masterPassword);

        // Save vault contents to disk (auto-encrypts)
        bool Save(const std::string& masterPassword) const;

        // Access raw vault data
        void SetData(const std::string& data) { mData = data; }
        const std::string& GetData() const { return mData; }

        // Change the vault file location
        void SetFilePath(const std::string& path) { mFilePath = path; }
        const std::string& GetFilePath() const { return mFilePath; }
    };
}
