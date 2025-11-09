// ============================================================================
// HiveMnd Utility Library - Security Module
// File: CryptoUtils.h
//
// What this does (plain-English):
//  - AES-256 encryption/decryption in CTR mode (no padding issues, any length)
//  - Random 256-bit key and 128-bit IV helpers (CSPRNG via std::random_device)
//  - Base64 helpers so we can return/accept ciphertext as printable strings
//
// Why CTR mode?
//  - Simple and fast for general data. We XOR plaintext with a keystream that
//    comes from encrypting a counter block (IV||counter) using AES-256.
//  - No padding headaches. Decryption mirrors encryption.
//
// Notes:
//  - This is dependency-free and portable. The AES core is adapted from the
//    public-domain "tiny-AES" project (credit: https://github.com/kokke/tiny-AES-c)
//  - We expose simple helpers: EncryptToBase64 / DecryptFromBase64.
//  - We purposely keep “key ownership” with the caller. You generate/store the
//    32-byte key (see GenerateRandomKey) and pass it in when needed.
//
// Security reminders (for future-self):
//  - NEVER reuse the same (key, IV) pair. We generate a fresh IV each encrypt.
//  - Store/ship IV alongside ciphertext (we prepend it; it’s not secret).
//  - If you need password-based keys, add PBKDF2/Argon2 in a higher-level util.
// ============================================================================

#pragma once
#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace HiveMnd::Security
{
    // 256-bit AES key (32 bytes)
    using Aes256Key = std::array<std::uint8_t, 32>;
    // 128-bit IV/nonce for CTR mode (16 bytes)
    using AesIV = std::array<std::uint8_t, 16>;

    // -----------------------------
    // Random material helpers
    // -----------------------------
    // Generate a cryptographically-strong random 256-bit key.
    Aes256Key GenerateRandomKey();

    // Generate a fresh random 128-bit IV (nonce) for CTR.
    AesIV GenerateRandomIV();

    // -----------------------------
    // Base64 helpers (for convenience)
    // -----------------------------
    std::string Base64Encode(const std::vector<std::uint8_t>& data);
    std::vector<std::uint8_t> Base64Decode(const std::string& b64);

    // -----------------------------
    // AES-256 CTR (core wrappers)
    // -----------------------------
    // Encrypts raw bytes with AES-256 in CTR mode using (key, iv).
    // Returns ciphertext bytes (same length as plaintext).
    std::vector<std::uint8_t> Aes256CtrEncrypt(const std::vector<std::uint8_t>& plaintext,
        const Aes256Key& key,
        const AesIV& iv);

    // Decrypts raw bytes with AES-256 in CTR mode using (key, iv).
    // CTR decrypt == encrypt (XOR with same keystream), kept separate for clarity.
    std::vector<std::uint8_t> Aes256CtrDecrypt(const std::vector<std::uint8_t>& ciphertext,
        const Aes256Key& key,
        const AesIV& iv);

    // -----------------------------
    // High-level convenience
    // -----------------------------
    // Encrypts text; returns Base64( IV || CIPHERTEXT )
    // Layout: [16 bytes IV][N bytes cipher] -> all Base64'ed to a string.
    std::string EncryptToBase64(const std::string& plaintextUtf8, const Aes256Key& key);

    // Accepts Base64( IV || CIPHERTEXT ); returns original plaintext string.
    // Throws std::runtime_error on malformed input.
    std::string DecryptFromBase64(const std::string& ivPlusCipher_b64, const Aes256Key& key);
}
