// ============================================================================
// HiveMnd Utility Library - Security Module
// File: PasswordVaultUtils.cpp
//
// Student-style notes:
//  - The vault is a std::unordered_map<service, {username, password}>.
//  - All data is serialized into a plaintext string like:
//       "service|username|password\n"
//  - Before saving, EncryptWithPassword() is used to encrypt the blob.
//  - Before loading, DecryptWithPassword() decrypts it back into entries.
//
//  - This demonstrates integrating PBKDF2 + AES for secure local storage.
// ============================================================================

#include "PasswordVaultUtils.h"
#include "PasswordCrypto.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

namespace HiveMnd::Security
{
    PasswordVault::PasswordVault(const std::string& path)
        : mFilePath(path)
    {
    }

    bool PasswordVault::Load(const std::string& masterPassword)
    {
        std::ifstream file(mFilePath, std::ios::binary);
        if (!file.is_open())
            return false;

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string encryptedData = buffer.str();

        try
        {
            std::string decrypted = DecryptWithPassword(encryptedData, masterPassword);

            mCredentials.clear();
            std::istringstream iss(decrypted);
            std::string line;
            while (std::getline(iss, line))
            {
                size_t p1 = line.find('|');
                size_t p2 = line.find('|', p1 + 1);
                if (p1 == std::string::npos || p2 == std::string::npos)
                    continue;

                std::string service = line.substr(0, p1);
                std::string username = line.substr(p1 + 1, p2 - p1 - 1);
                std::string password = line.substr(p2 + 1);
                mCredentials[service] = { username, password };
            }

            return true;
        }
        catch (...)
        {
            std::cerr << "[Vault] Decryption failed. Wrong password or corrupted file.\n";
            return false;
        }
    }

    bool PasswordVault::Save(const std::string& masterPassword) const
    {
        std::ostringstream oss;
        for (const auto& [service, cred] : mCredentials)
        {
            oss << service << "|" << cred.username << "|" << cred.password << "\n";
        }

        std::string encrypted = EncryptWithPassword(oss.str(), masterPassword);

        std::ofstream file(mFilePath, std::ios::binary);
        if (!file.is_open())
            return false;

        file << encrypted;
        return true;
    }

    void PasswordVault::AddCredential(const std::string& service,
        const std::string& username,
        const std::string& password)
    {
        mCredentials[service] = { username, password };
    }

    void PasswordVault::RemoveCredential(const std::string& service)
    {
        mCredentials.erase(service);
    }

    Credential PasswordVault::GetCredential(const std::string& service) const
    {
        auto it = mCredentials.find(service);
        if (it == mCredentials.end())
            throw std::runtime_error("Service not found: " + service);
        return it->second;
    }

    std::vector<std::string> PasswordVault::ListServices() const
    {
        std::vector<std::string> result;
        result.reserve(mCredentials.size());
        for (const auto& [service, _] : mCredentials)
            result.push_back(service);
        return result;
    }

    void PasswordVault::Clear()
    {
        mCredentials.clear();
    }
}
