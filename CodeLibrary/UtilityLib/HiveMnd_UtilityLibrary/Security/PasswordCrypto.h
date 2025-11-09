// ============================================================================
// HiveMnd Utility Library - Security Module
// File: PasswordCrypto.h
//
// What this does:
//  • Provides password-based encryption using PBKDF2-HMAC-SHA256 + AES-256-CTR.
//  • Derives a 256-bit key from the password and a random salt.
//  • Generates a random IV for AES encryption.
//  • Encodes (salt || IV || ciphertext) as a single Base64 string for storage.
//
// Security overview:
//  - AES-256 in CTR mode (same as CryptoUtils).
//  - PBKDF2 with configurable iterations (default 200,000).
//  - Salt and IV are stored in the output (not secret).
//
// Usage example:
//    std::string cipher = EncryptWithPassword("secret text", "myPass123");
//    std::string plain  = DecryptWithPassword(cipher, "myPass123");
//
//  Output layout (Base64 encoded):
//  [16 bytes Salt][16 bytes IV][N bytes Ciphertext]
// ============================================================================

#pragma once
#include <string>

namespace HiveMnd::Security
{
    // Encrypts plaintext with password → returns Base64(salt+iv+ciphertext)
    std::string EncryptWithPassword(const std::string& plaintext,
        const std::string& password,
        unsigned int iterations = 200000);

    // Decrypts Base64(salt+iv+ciphertext) with password → plaintext
    std::string DecryptWithPassword(const std::string& b64Data,
        const std::string& password,
        unsigned int iterations = 200000);
}

