// ============================================================================
// HiveMnd Utility Library - Security Module
// File: PasswordVaultUtils.h
//
// Description:
//  A simple secure credential vault for HiveMnd applications.
//  Uses AES-256 + PBKDF2-HMAC-SHA256 encryption (via PasswordCrypto).
//
// Features:
//  - Securely stores service credentials (service → username/password).
//  - Encrypted file storage (salt + IV handled automatically).
//  - Supports add/remove/get operations.
//  - Portable text-based format using Base64.
//
// Security Summary:
//  - All sensitive data (usernames, passwords) are encrypted together
//    using the master password.
//  - Vault file can be safely synced or shared as long as the master
//    password remains secret.
//  - SHA256-HMAC integrity could be added later for tamper detection.
//
// Usage Example:
//     HiveMnd::Security::PasswordVault vault("user_vault.hvault");
//     vault.Load("MyMasterPassword");
//     vault.AddCredential("GitHub", "JonSmith", "MyPass123!");
//     vault.Save("MyMasterPassword");
//     std::cout << vault.GetCredential("GitHub").username << std::endl;
// ============================================================================

#pragma once
#include <string>
#include <unordered_map>

namespace HiveMnd::Security
{
    struct Credential
    {
        std::string username;
        std::string password;
    };

    class PasswordVault
    {
    private:
        std::unordered_map<std::string, Credential> mCredentials;
        std::string mFilePath;

    public:
        PasswordVault(const std::string& path);

        // Loads vault from disk and decrypts it
        bool Load(const std::string& masterPassword);

        // Saves encrypted vault back to disk
        bool Save(const std::string& masterPassword) const;

        // Add or update a credential
        void AddCredential(const std::string& service,
            const std::string& username,
            const std::string& password);

        // Remove a credential
        void RemoveCredential(const std::string& service);

        // Fetch a stored credential (throws if not found)
        Credential GetCredential(const std::string& service) const;

        // Lists all saved services
        std::vector<std::string> ListServices() const;

        // Clears all credentials (memory only)
        void Clear();
    };
}

