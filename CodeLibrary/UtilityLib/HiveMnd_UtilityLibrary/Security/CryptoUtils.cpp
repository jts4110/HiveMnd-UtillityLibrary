// ============================================================================
// HiveMnd Utility Library - Security Module
// File: CryptoUtils.cpp
//
// Implementation notes (student-style):
//  1) AES core:
//     - This is a small, public-domain AES-256 implementation adapted from
//       tiny-AES (https://github.com/kokke/tiny-AES-c). We keep only what we
//       need (AES-256 ECB encrypt of a single 16-byte block) to build CTR.
//  2) CTR mode:
//     - We build a 16-byte counter block:  [ 16-byte IV ] where the last
//       8 bytes act as an incrementing counter. Each block:
//          keystream = AES_Encrypt(counter_block)
//          output[i..i+15] = input[i..i+15] XOR keystream
//       Then we ++counter (little-endian increment of last 8 bytes).
//  3) Base64:
//     - Simple encode/decode so callers can pass ciphertext around as text.
//  4) Randomness:
//     - std::random_device + std::uniform_int_distribution<uint32_t> to fill
//       key/iv. On Windows this sources from BCryptGenRandom under the hood.
//
// Important: Never reuse (key, IV) pair. We always generate a fresh IV in
// EncryptToBase64 and prepend it to the ciphertext so Decrypt can recover it.
// ============================================================================

#include "CryptoUtils.h"
#include <stdexcept>
#include <random>
#include <cstring>
#include <array>
#include <limits>

namespace HiveMnd::Security
{
    // -----------------------------
    // Random helpers
    // -----------------------------
    template <size_t N>
    static std::array<std::uint8_t, N> FillRandom()
    {
        std::array<std::uint8_t, N> out{};
        std::random_device rd;
        std::uniform_int_distribution<std::uint32_t> dist(0, std::numeric_limits<std::uint32_t>::max());
        size_t i = 0;
        while (i < N)
        {
            auto word = dist(rd);
            // copy 4 bytes at a time
            for (int b = 0; b < 4 && i < N; ++b, ++i)
                out[i] = static_cast<std::uint8_t>((word >> (8 * b)) & 0xFF);
        }
        return out;
    }

    Aes256Key GenerateRandomKey() { return FillRandom<32>(); }
    AesIV     GenerateRandomIV() { return FillRandom<16>(); }

    // -----------------------------
    // Base64 (small implementation)
    // -----------------------------
    static const char* B64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    std::string Base64Encode(const std::vector<std::uint8_t>& data)
    {
        std::string out;
        out.reserve(((data.size() + 2) / 3) * 4);

        size_t i = 0;
        while (i + 3 <= data.size())
        {
            uint32_t n = (data[i] << 16) | (data[i + 1] << 8) | data[i + 2];
            out.push_back(B64[(n >> 18) & 63]);
            out.push_back(B64[(n >> 12) & 63]);
            out.push_back(B64[(n >> 6) & 63]);
            out.push_back(B64[n & 63]);
            i += 3;
        }
        if (i < data.size())
        {
            uint32_t n = (data[i] << 16);
            out.push_back(B64[(n >> 18) & 63]);
            if (i + 1 < data.size())
            {
                n |= (data[i + 1] << 8);
                out.push_back(B64[(n >> 12) & 63]);
                out.push_back(B64[(n >> 6) & 63]);
                out.push_back('=');
            }
            else
            {
                out.push_back(B64[(n >> 12) & 63]);
                out.push_back('=');
                out.push_back('=');
            }
        }
        return out;
    }

    static inline int B64Val(char c)
    {
        if ('A' <= c && c <= 'Z') return c - 'A';
        if ('a' <= c && c <= 'z') return c - 'a' + 26;
        if ('0' <= c && c <= '9') return c - '0' + 52;
        if (c == '+') return 62;
        if (c == '/') return 63;
        return -1;
    }

    std::vector<std::uint8_t> Base64Decode(const std::string& b64)
    {
        if (b64.empty()) return {};

        size_t pad = 0;
        if (b64.size() >= 2 && b64[b64.size() - 1] == '=') { pad++; }
        if (b64.size() >= 2 && b64[b64.size() - 2] == '=') { pad++; }

        size_t blocks = b64.size() / 4;
        std::vector<std::uint8_t> out;
        out.reserve(blocks * 3);

        for (size_t i = 0; i < blocks; ++i)
        {
            int a = B64Val(b64[4 * i + 0]);
            int b = B64Val(b64[4 * i + 1]);
            int c = B64Val(b64[4 * i + 2]);
            int d = B64Val(b64[4 * i + 3]);

            if (a < 0 || b < 0 || (c < 0 && b64[4 * i + 2] != '=') || (d < 0 && b64[4 * i + 3] != '='))
                throw std::runtime_error("Invalid Base64 input");

            uint32_t n =
                ((a & 63) << 18) |
                ((b & 63) << 12) |
                (((c < 0) ? 0 : (c & 63)) << 6) |
                (((d < 0) ? 0 : (d & 63)));

            out.push_back((n >> 16) & 0xFF);
            if (b64[4 * i + 2] != '=') out.push_back((n >> 8) & 0xFF);
            if (b64[4 * i + 3] != '=') out.push_back(n & 0xFF);
        }
        return out;
    }

    // -----------------------------
    // AES-256 core (tiny-AES adapted)
    // Only ECB-Encrypt of a single 16-byte block is required for CTR mode.
    // -----------------------------

    namespace detail
    {
        // Rijndael S-box
        static const uint8_t sbox[256] = {
            // (omitted long table expanded in code)
            0x63,0x7c,0x77,0x7b,0xf2,0x6b,0x6f,0xc5,0x30,0x01,0x67,0x2b,0xfe,0xd7,0xab,0x76,
            0xca,0x82,0xc9,0x7d,0xfa,0x59,0x47,0xf0,0xad,0xd4,0xa2,0xaf,0x9c,0xa4,0x72,0xc0,
            0xb7,0xfd,0x93,0x26,0x36,0x3f,0xf7,0xcc,0x34,0xa5,0xe5,0xf1,0x71,0xd8,0x31,0x15,
            0x04,0xc7,0x23,0xc3,0x18,0x96,0x05,0x9a,0x07,0x12,0x80,0xe2,0xeb,0x27,0xb2,0x75,
            0x09,0x83,0x2c,0x1a,0x1b,0x6e,0x5a,0xa0,0x52,0x3b,0xd6,0xb3,0x29,0xe3,0x2f,0x84,
            0x53,0xd1,0x00,0xed,0x20,0xfc,0xb1,0x5b,0x6a,0xcb,0xbe,0x39,0x4a,0x4c,0x58,0xcf,
            0xd0,0xef,0xaa,0xfb,0x43,0x4d,0x33,0x85,0x45,0xf9,0x02,0x7f,0x50,0x3c,0x9f,0xa8,
            0x51,0xa3,0x40,0x8f,0x92,0x9d,0x38,0xf5,0xbc,0xb6,0xda,0x21,0x10,0xff,0xf3,0xd2,
            0xcd,0x0c,0x13,0xec,0x5f,0x97,0x44,0x17,0xc4,0xa7,0x7e,0x3d,0x64,0x5d,0x19,0x73,
            0x60,0x81,0x4f,0xdc,0x22,0x2a,0x90,0x88,0x46,0xee,0xb8,0x14,0xde,0x5e,0x0b,0xdb,
            0xe0,0x32,0x3a,0x0a,0x49,0x06,0x24,0x5c,0xc2,0xd3,0xac,0x62,0x91,0x95,0xe4,0x79,
            0xe7,0xc8,0x37,0x6d,0x8d,0xd5,0x4e,0xa9,0x6c,0x56,0xf4,0xea,0x65,0x7a,0xae,0x08,
            0xba,0x78,0x25,0x2e,0x1c,0xa6,0xb4,0xc6,0xe8,0xdd,0x74,0x1f,0x4b,0xbd,0x8b,0x8a,
            0x70,0x3e,0xb5,0x66,0x48,0x03,0xf6,0x0e,0x61,0x35,0x57,0xb9,0x86,0xc1,0x1d,0x9e,
            0xe1,0xf8,0x98,0x11,0x69,0xd9,0x8e,0x94,0x9b,0x1e,0x87,0xe9,0xce,0x55,0x28,0xdf,
            0x8c,0xa1,0x89,0x0d,0xbf,0xe6,0x42,0x68,0x41,0x99,0x2d,0x0f,0xb0,0x54,0xbb,0x16
        };

        static const uint8_t Rcon[255] = {
            0x8d,0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x1b,0x36,
            // (rest unused for AES-256 beyond needed rounds; kept minimal)
        };

        struct AES256Ctx
        {
            uint8_t RoundKey[240]; // 14 rounds -> 240 bytes
        };

        static void KeyExpansion(const uint8_t* Key, AES256Ctx& ctx)
        {
            // Expand 32-byte key into 240-byte round key schedule.
            const uint8_t Nk = 8;   // words in key
            const uint8_t Nb = 4;   // words in block
            const uint8_t Nr = 14;  // rounds

            memcpy(ctx.RoundKey, Key, 32);
            uint32_t bytesGenerated = 32;
            uint8_t rconIter = 1;
            uint8_t temp[4];

            auto SubWord = [](uint8_t* w) {
                w[0] = sbox[w[0]]; w[1] = sbox[w[1]];
                w[2] = sbox[w[2]]; w[3] = sbox[w[3]];
                };

            while (bytesGenerated < (Nb * (Nr + 1) * 4))
            {
                for (int i = 0; i < 4; ++i)
                    temp[i] = ctx.RoundKey[bytesGenerated - 4 + i];

                if (bytesGenerated % (Nk * 4) == 0)
                {
                    // RotWord
                    uint8_t t = temp[0];
                    temp[0] = temp[1]; temp[1] = temp[2];
                    temp[2] = temp[3]; temp[3] = t;
                    // SubWord
                    SubWord(temp);
                    // Rcon
                    temp[0] ^= (uint8_t)(1 << (rconIter - 1));
                    rconIter++;
                }
                else if (bytesGenerated % (Nk * 4) == 16)
                {
                    // For AES-256: extra SubWord step
                    SubWord(temp);
                }

                for (int i = 0; i < 4; ++i)
                {
                    ctx.RoundKey[bytesGenerated] =
                        ctx.RoundKey[bytesGenerated - (Nk * 4)] ^ temp[i];
                    bytesGenerated++;
                }
            }
        }

        // Galois multiply by 2 in GF(2^8)
        static inline uint8_t xtime(uint8_t x) { return (uint8_t)((x << 1) ^ ((x & 0x80) ? 0x1b : 0x00)); }

        static void SubBytes(uint8_t* state)
        {
            for (int i = 0; i < 16; ++i) state[i] = sbox[state[i]];
        }

        static void ShiftRows(uint8_t* s)
        {
            uint8_t t;

            // row 1
            t = s[1]; s[1] = s[5]; s[5] = s[9]; s[9] = s[13]; s[13] = t;
            // row 2
            t = s[2]; s[2] = s[10]; s[10] = t; t = s[6]; s[6] = s[14]; s[14] = t;
            // row 3
            t = s[3]; s[3] = s[15]; s[15] = s[11]; s[11] = s[7]; s[7] = t;
        }

        static void MixColumns(uint8_t* s)
        {
            for (int i = 0; i < 4; ++i)
            {
                int idx = i * 4;
                uint8_t a0 = s[idx + 0], a1 = s[idx + 1], a2 = s[idx + 2], a3 = s[idx + 3];
                uint8_t r0 = (uint8_t)(xtime(a0) ^ (xtime(a1) ^ a1) ^ a2 ^ a3);
                uint8_t r1 = (uint8_t)(a0 ^ xtime(a1) ^ (xtime(a2) ^ a2) ^ a3);
                uint8_t r2 = (uint8_t)(a0 ^ a1 ^ xtime(a2) ^ (xtime(a3) ^ a3));
                uint8_t r3 = (uint8_t)((xtime(a0) ^ a0) ^ a1 ^ a2 ^ xtime(a3));
                s[idx + 0] = r0; s[idx + 1] = r1; s[idx + 2] = r2; s[idx + 3] = r3;
            }
        }

        static void AddRoundKey(uint8_t round, uint8_t* state, const AES256Ctx& ctx)
        {
            for (int i = 0; i < 16; ++i)
                state[i] ^= ctx.RoundKey[(round * 16) + i];
        }

        static void AES_ECB_encrypt(const uint8_t* input, uint8_t* output, const AES256Ctx& ctx)
        {
            // Copy input -> state
            uint8_t state[16];
            memcpy(state, input, 16);

            const uint8_t Nr = 14;

            AddRoundKey(0, state, ctx);

            for (uint8_t round = 1; round < Nr; ++round)
            {
                SubBytes(state);
                ShiftRows(state);
                MixColumns(state);
                AddRoundKey(round, state, ctx);
            }

            SubBytes(state);
            ShiftRows(state);
            AddRoundKey(Nr, state, ctx);

            memcpy(output, state, 16);
        }
    } // namespace detail

    // -----------------------------
    // CTR mode helpers
    // -----------------------------
    static void IncrementCounter(std::array<uint8_t, 16>& counter)
    {
        // Increment last 8 bytes as little-endian integer
        for (int i = 15; i >= 8; --i)
        {
            uint16_t sum = (uint16_t)counter[i] + 1;
            counter[i] = (uint8_t)(sum & 0xFF);
            if ((sum & 0xFF) != 0) break; // stop carry if no overflow
        }
    }

    static std::vector<std::uint8_t> AesCtrXor(const std::vector<uint8_t>& in,
        const Aes256Key& key,
        const AesIV& iv)
    {
        // 1) Prepare AES round keys
        detail::AES256Ctx ctx{};
        detail::KeyExpansion(key.data(), ctx);

        // 2) Make initial counter block = IV (we will increment the last 8 bytes)
        std::array<uint8_t, 16> counter{};
        memcpy(counter.data(), iv.data(), 16);

        // 3) Process in 16-byte chunks
        std::vector<uint8_t> out(in.size());
        uint8_t keystream[16];

        size_t offset = 0;
        while (offset < in.size())
        {
            // Encrypt counter -> keystream
            detail::AES_ECB_encrypt(counter.data(), keystream, ctx);

            // XOR block
            size_t remain = in.size() - offset;
            size_t n = remain < 16 ? remain : 16;
            for (size_t i = 0; i < n; ++i)
                out[offset + i] = in[offset + i] ^ keystream[i];

            offset += n;
            IncrementCounter(counter);
        }
        return out;
    }

    std::vector<std::uint8_t> Aes256CtrEncrypt(const std::vector<std::uint8_t>& plaintext,
        const Aes256Key& key,
        const AesIV& iv)
    {
        return AesCtrXor(plaintext, key, iv);
    }

    std::vector<std::uint8_t> Aes256CtrDecrypt(const std::vector<std::uint8_t>& ciphertext,
        const Aes256Key& key,
        const AesIV& iv)
    {
        // CTR decrypt is the same operation
        return AesCtrXor(ciphertext, key, iv);
    }

    // -----------------------------
    // High-level convenience
    // -----------------------------
    std::string EncryptToBase64(const std::string& plaintextUtf8, const Aes256Key& key)
    {
        // 1) Fresh IV
        AesIV iv = GenerateRandomIV();

        // 2) Encrypt
        std::vector<uint8_t> plain(plaintextUtf8.begin(), plaintextUtf8.end());
        std::vector<uint8_t> cipher = Aes256CtrEncrypt(plain, key, iv);

        // 3) Prepend IV to ciphertext and Base64
        std::vector<uint8_t> iv_plus;
        iv_plus.reserve(16 + cipher.size());
        iv_plus.insert(iv_plus.end(), iv.begin(), iv.end());
        iv_plus.insert(iv_plus.end(), cipher.begin(), cipher.end());

        return Base64Encode(iv_plus);
    }

    std::string DecryptFromBase64(const std::string& ivPlusCipher_b64, const Aes256Key& key)
    {
        // 1) Base64 decode and validate
        std::vector<uint8_t> blob = Base64Decode(ivPlusCipher_b64);
        if (blob.size() < 16) throw std::runtime_error("Cipher blob too small (missing IV).");

        // 2) Split IV | CIPHER
        AesIV iv{};
        memcpy(iv.data(), blob.data(), 16);
        std::vector<uint8_t> cipher(blob.begin() + 16, blob.end());

        // 3) Decrypt and return as string
        std::vector<uint8_t> plain = Aes256CtrDecrypt(cipher, key, iv);
        return std::string(plain.begin(), plain.end());
    }
}
