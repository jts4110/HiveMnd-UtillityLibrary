// ============================================================================
// HiveMnd Utility Library - Security Module
// File: PasswordCrypto.cpp
//
// Implementation notes:
//  - Combines PBKDF2 (HashUtils) and AES-256 CTR (CryptoUtils).
//  - Uses 16-byte random salt and IV.
//  - Encodes full result as Base64(salt||iv||ciphertext).
// ============================================================================

#include "PasswordCrypto.h"
#include "CryptoUtils.h"
#include "HashUtils.h"
#include <vector>
#include <cstring>
#include <stdexcept>

namespace HiveMnd::Security
{
    std::string EncryptWithPassword(const std::string& plaintext,
        const std::string& password,
        unsigned int iterations)
    {
        // 1) Generate random salt + IV
        AesIV salt = GenerateRandomIV(); // 16 bytes of salt
        AesIV iv = GenerateRandomIV(); // 16 bytes for AES IV

        // 2) Derive key from password + salt using PBKDF2-HMAC-SHA256
        std::vector<std::uint8_t> saltVec(salt.begin(), salt.end());
        auto keyBytes = PBKDF2_HMAC_SHA256(password, saltVec, iterations, 32);
        Aes256Key key;
        std::memcpy(key.data(), keyBytes.data(), 32);

        // 3) Encrypt using AES-256-CTR
        std::vector<std::uint8_t> plain(plaintext.begin(), plaintext.end());
        std::vector<std::uint8_t> cipher = Aes256CtrEncrypt(plain, key, iv);

        // 4) Combine salt + iv + cipher
        std::vector<std::uint8_t> full;
        full.reserve(32 + cipher.size());
        full.insert(full.end(), salt.begin(), salt.end());
        full.insert(full.end(), iv.begin(), iv.end());
        full.insert(full.end(), cipher.begin(), cipher.end());

        // 5) Base64 encode the result
        return Base64Encode(full);
    }

    std::string DecryptWithPassword(const std::string& b64Data,
        const std::string& password,
        unsigned int iterations)
    {
        // 1) Base64 decode
        std::vector<std::uint8_t> blob = Base64Decode(b64Data);
        if (blob.size() < 32)
            throw std::runtime_error("Invalid encrypted data: too short");

        // 2) Extract salt + IV + ciphertext
        AesIV salt{}, iv{};
        std::memcpy(salt.data(), blob.data(), 16);
        std::memcpy(iv.data(), blob.data() + 16, 16);

        std::vector<std::uint8_t> cipher(blob.begin() + 32, blob.end());

        // 3) Derive key again using same salt + password
        std::vector<std::uint8_t> saltVec(salt.begin(), salt.end());
        auto keyBytes = PBKDF2_HMAC_SHA256(password, saltVec, iterations, 32);
        Aes256Key key;
        std::memcpy(key.data(), keyBytes.data(), 32);

        // 4) Decrypt
        std::vector<std::uint8_t> plain = Aes256CtrDecrypt(cipher, key, iv);
        return std::string(plain.begin(), plain.end());
    }
}
