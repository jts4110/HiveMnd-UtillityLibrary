// ============================================================================
// HiveMnd Utility Library - Security Module
// File: HashUtils.h
//
// What this provides (plain-English):
//  • SHA-256: general-purpose cryptographic hash (32 bytes)
//  • HMAC-SHA256: keyed hash for message authentication
//  • PBKDF2-HMAC-SHA256: password → key derivation (iterated HMAC)
//
// Why this exists:
//  - We need hashes for integrity checks, message auth, and to derive strong
//    AES keys from passwords (salt + many iterations).
//
// Notes:
//  - All implementations are dependency-free and portable.
//  - Hex helpers are included for easy logging/serialization.
//  - Use PBKDF2 to turn user passwords into a 32-byte AES key for CryptoUtils.
//
// Security reminders:
//  - Always use a random salt for PBKDF2 (at least 16 bytes).
//  - Use a high iteration count (e.g., 100k – 600k depending on perf target).
//  - For new designs prefer Argon2/scrypt when available; PBKDF2 remains
//    widely accepted and FIPS-friendly.
// ============================================================================

#pragma once
#include <array>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace HiveMnd::Security
{
    // ---- SHA-256 ------------------------------------------------------------
    // Returns 32 raw bytes of the SHA-256 digest.
    std::array<std::uint8_t, 32> SHA256(const std::vector<std::uint8_t>& data);
    std::array<std::uint8_t, 32> SHA256(std::string_view text);

    // Hex-encoded (lowercase) helper for quick displays/logging.
    std::string SHA256Hex(const std::vector<std::uint8_t>& data);
    std::string SHA256Hex(std::string_view text);

    // ---- HMAC-SHA256 --------------------------------------------------------
    // Standard HMAC using SHA-256. Returns 32-byte tag.
    std::array<std::uint8_t, 32> HMAC_SHA256(const std::vector<std::uint8_t>& key,
        const std::vector<std::uint8_t>& message);
    std::string HMAC_SHA256_Hex(const std::vector<std::uint8_t>& key,
        const std::vector<std::uint8_t>& message);

    // ---- PBKDF2-HMAC-SHA256 -------------------------------------------------
    // Derives 'dkLen' bytes from a password, salt, and iteration count.
    // Throws on dkLen == 0 or iterations == 0.
    std::vector<std::uint8_t> PBKDF2_HMAC_SHA256(std::string_view password,
        const std::vector<std::uint8_t>& salt,
        std::uint32_t iterations,
        std::size_t dkLen);
}

