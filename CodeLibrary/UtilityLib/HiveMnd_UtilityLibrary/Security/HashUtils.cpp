// ============================================================================
// HiveMnd Utility Library - Security Module
// File: HashUtils.cpp
//
// Student-style notes:
//  1) SHA-256:
//     - Classic Merkle–Damgård construction over 512-bit blocks.
//     - We implement the compression function with the standard K constants.
//     - Input is padded with '1' bit + zeros + 64-bit big-endian length.
//
//  2) HMAC-SHA256:
//     - HMAC(K, m) = H( (K⊕opad) || H((K⊕ipad)||m) )
//     - Blocksize for SHA-256 is 64 bytes. Long keys are hashed down first.
//
//  3) PBKDF2-HMAC-SHA256:
//     - F(P,S,c,i) = U1 ⊕ U2 ⊕ ... ⊕ Uc
//       U1 = HMAC(P, S || INT_32_BE(i)), Uj = HMAC(P, U_{j-1})
//     - We concatenate blocks until we produce dkLen bytes.
//
// All code here is dependency-free and aims to be clear and educational.
// ============================================================================

#include "HashUtils.h"
#include <cstring>
#include <stdexcept>
#include <sstream>
#include <iomanip>

namespace HiveMnd::Security
{
    // --------------------------
    // Helpers (hex + ROTR etc.)
    // --------------------------
    static inline std::uint32_t ROTR(std::uint32_t x, std::uint32_t n)
    {
        return (x >> n) | (x << (32U - n));
    }

    static std::string ToHex(const std::uint8_t* bytes, std::size_t len)
    {
        std::ostringstream oss;
        oss << std::hex << std::setfill('0');
        for (std::size_t i = 0; i < len; ++i)
            oss << std::setw(2) << static_cast<int>(bytes[i]);
        return oss.str();
    }

    // --------------------------
    // SHA-256 implementation
    // --------------------------
    namespace sha256_detail
    {
        static constexpr std::uint32_t K[64] = {
            0x428a2f98U,0x71374491U,0xb5c0fbcfU,0xe9b5dba5U,0x3956c25bU,0x59f111f1U,0x923f82a4U,0xab1c5ed5U,
            0xd807aa98U,0x12835b01U,0x243185beU,0x550c7dc3U,0x72be5d74U,0x80deb1feU,0x9bdc06a7U,0xc19bf174U,
            0xe49b69c1U,0xefbe4786U,0x0fc19dc6U,0x240ca1ccU,0x2de92c6fU,0x4a7484aaU,0x5cb0a9dcU,0x76f988daU,
            0x983e5152U,0xa831c66dU,0xb00327c8U,0xbf597fc7U,0xc6e00bf3U,0xd5a79147U,0x06ca6351U,0x14292967U,
            0x27b70a85U,0x2e1b2138U,0x4d2c6dfcU,0x53380d13U,0x650a7354U,0x766a0abbU,0x81c2c92eU,0x92722c85U,
            0xa2bfe8a1U,0xa81a664bU,0xc24b8b70U,0xc76c51a3U,0xd192e819U,0xd6990624U,0xf40e3585U,0x106aa070U,
            0x19a4c116U,0x1e376c08U,0x2748774cU,0x34b0bcb5U,0x391c0cb3U,0x4ed8aa4aU,0x5b9cca4fU,0x682e6ff3U,
            0x748f82eeU,0x78a5636fU,0x84c87814U,0x8cc70208U,0x90befffaU,0xa4506cebU,0xbef9a3f7U,0xc67178f2U
        };

        struct Ctx
        {
            std::uint32_t state[8];
            std::uint64_t bitlen; // total message length in bits
            std::uint8_t  buffer[64];
            std::size_t   bufferLen;
        };

        static void Init(Ctx& c)
        {
            c.state[0] = 0x6a09e667U; c.state[1] = 0xbb67ae85U; c.state[2] = 0x3c6ef372U; c.state[3] = 0xa54ff53aU;
            c.state[4] = 0x510e527fU; c.state[5] = 0x9b05688cU; c.state[6] = 0x1f83d9abU; c.state[7] = 0x5be0cd19U;
            c.bitlen = 0;
            c.bufferLen = 0;
        }

        static void Transform(Ctx& c, const std::uint8_t block[64])
        {
            std::uint32_t w[64];
            for (int i = 0; i < 16; ++i)
            {
                w[i] = (block[4 * i + 0] << 24) |
                    (block[4 * i + 1] << 16) |
                    (block[4 * i + 2] << 8) |
                    (block[4 * i + 3]);
            }
            for (int i = 16; i < 64; ++i)
            {
                std::uint32_t s0 = ROTR(w[i - 15], 7) ^ ROTR(w[i - 15], 18) ^ (w[i - 15] >> 3);
                std::uint32_t s1 = ROTR(w[i - 2], 17) ^ ROTR(w[i - 2], 19) ^ (w[i - 2] >> 10);
                w[i] = w[i - 16] + s0 + w[i - 7] + s1;
            }

            std::uint32_t a = c.state[0], b = c.state[1], ccc = c.state[2], d = c.state[3];
            std::uint32_t e = c.state[4], f = c.state[5], g = c.state[6], h = c.state[7];

            for (int i = 0; i < 64; ++i)
            {
                std::uint32_t S1 = ROTR(e, 6) ^ ROTR(e, 11) ^ ROTR(e, 25);
                std::uint32_t ch = (e & f) ^ ((~e) & g);
                std::uint32_t temp1 = h + S1 + ch + K[i] + w[i];
                std::uint32_t S0 = ROTR(a, 2) ^ ROTR(a, 13) ^ ROTR(a, 22);
                std::uint32_t maj = (a & ccc) ^ (a & d) ^ (ccc & d);
                std::uint32_t temp2 = S0 + maj;

                h = g; g = f; f = e;
                e = d + temp1;
                d = ccc; ccc = b; b = a;
                a = temp1 + temp2;
            }

            c.state[0] += a; c.state[1] += b; c.state[2] += ccc; c.state[3] += d;
            c.state[4] += e; c.state[5] += f; c.state[6] += g; c.state[7] += h;
        }

        static void Update(Ctx& c, const std::uint8_t* data, std::size_t len)
        {
            c.bitlen += static_cast<std::uint64_t>(len) * 8ULL;

            // If buffer has leftover, fill it first
            if (c.bufferLen)
            {
                std::size_t toCopy = std::min(len, 64 - c.bufferLen);
                std::memcpy(c.buffer + c.bufferLen, data, toCopy);
                c.bufferLen += toCopy;
                data += toCopy;
                len -= toCopy;

                if (c.bufferLen == 64)
                {
                    Transform(c, c.buffer);
                    c.bufferLen = 0;
                }
            }

            // Process full 64-byte blocks
            while (len >= 64)
            {
                Transform(c, data);
                data += 64;
                len -= 64;
            }

            // Buffer the rest
            if (len > 0)
            {
                std::memcpy(c.buffer, data, len);
                c.bufferLen = len;
            }
        }

        static void Final(Ctx& c, std::uint8_t out[32])
        {
            // Pad: 0x80, then zeros, then 64-bit big-endian length
            std::uint8_t pad[64] = { 0x80 };
            std::size_t padLen = (c.bufferLen < 56) ? (56 - c.bufferLen) : (64 + 56 - c.bufferLen);

            Update(c, pad, padLen);

            std::uint8_t beLen[8];
            for (int i = 0; i < 8; ++i)
                beLen[7 - i] = static_cast<std::uint8_t>((c.bitlen >> (8 * i)) & 0xFF);

            Update(c, beLen, 8);

            for (int i = 0; i < 8; ++i)
            {
                out[4 * i + 0] = static_cast<std::uint8_t>((c.state[i] >> 24) & 0xFF);
                out[4 * i + 1] = static_cast<std::uint8_t>((c.state[i] >> 16) & 0xFF);
                out[4 * i + 2] = static_cast<std::uint8_t>((c.state[i] >> 8) & 0xFF);
                out[4 * i + 3] = static_cast<std::uint8_t>((c.state[i] >> 0) & 0xFF);
            }
        }
    } // namespace sha256_detail

    std::array<std::uint8_t, 32> SHA256(const std::vector<std::uint8_t>& data)
    {
        sha256_detail::Ctx c;
        sha256_detail::Init(c);
        if (!data.empty())
            sha256_detail::Update(c, data.data(), data.size());

        std::array<std::uint8_t, 32> out{};
        sha256_detail::Final(c, out.data());
        return out;
    }

    std::array<std::uint8_t, 32> SHA256(std::string_view text)
    {
        sha256_detail::Ctx c;
        sha256_detail::Init(c);
        if (!text.empty())
            sha256_detail::Update(c, reinterpret_cast<const std::uint8_t*>(text.data()), text.size());

        std::array<std::uint8_t, 32> out{};
        sha256_detail::Final(c, out.data());
        return out;
    }

    std::string SHA256Hex(const std::vector<std::uint8_t>& data)
    {
        auto d = SHA256(data);
        return ToHex(d.data(), d.size());
    }

    std::string SHA256Hex(std::string_view text)
    {
        auto d = SHA256(text);
        return ToHex(d.data(), d.size());
    }

    // --------------------------
    // HMAC-SHA256
    // --------------------------
    std::array<std::uint8_t, 32> HMAC_SHA256(const std::vector<std::uint8_t>& key,
        const std::vector<std::uint8_t>& message)
    {
        // SHA-256 block size is 64 bytes
        const std::size_t blockSize = 64;

        // 1) Key normalization: if key > blockSize → hash it; else pad with zeros
        std::vector<std::uint8_t> K;
        if (key.size() > blockSize)
        {
            auto kh = SHA256(key);
            K.assign(kh.begin(), kh.end());
        }
        else
        {
            K = key;
        }
        K.resize(blockSize, 0x00);

        // 2) ipad/opad
        std::vector<std::uint8_t> ipad(blockSize), opad(blockSize);
        for (std::size_t i = 0; i < blockSize; ++i)
        {
            ipad[i] = static_cast<std::uint8_t>(K[i] ^ 0x36);
            opad[i] = static_cast<std::uint8_t>(K[i] ^ 0x5c);
        }

        // 3) inner = H(ipad || message)
        std::vector<std::uint8_t> inner; inner.reserve(blockSize + message.size());
        inner.insert(inner.end(), ipad.begin(), ipad.end());
        inner.insert(inner.end(), message.begin(), message.end());
        auto innerHash = SHA256(inner);

        // 4) outer = H(opad || innerHash)
        std::vector<std::uint8_t> outer; outer.reserve(blockSize + innerHash.size());
        outer.insert(outer.end(), opad.begin(), opad.end());
        outer.insert(outer.end(), innerHash.begin(), innerHash.end());

        auto mac = SHA256(outer);
        return mac;
    }

    std::string HMAC_SHA256_Hex(const std::vector<std::uint8_t>& key,
        const std::vector<std::uint8_t>& message)
    {
        auto mac = HMAC_SHA256(key, message);
        return ToHex(mac.data(), mac.size());
    }

    // --------------------------
    // PBKDF2-HMAC-SHA256
    // --------------------------
    std::vector<std::uint8_t> PBKDF2_HMAC_SHA256(std::string_view password,
        const std::vector<std::uint8_t>& salt,
        std::uint32_t iterations,
        std::size_t dkLen)
    {
        if (iterations == 0)  throw std::invalid_argument("PBKDF2 iterations must be > 0");
        if (dkLen == 0)       throw std::invalid_argument("PBKDF2 dkLen must be > 0");

        // Convert password to bytes once
        std::vector<std::uint8_t> P(password.begin(), password.end());

        const std::size_t hLen = 32; // SHA-256 output size
        std::size_t l = (dkLen + hLen - 1) / hLen; // number of blocks
        std::size_t r = dkLen - (l - 1) * hLen;    // bytes in last block

        std::vector<std::uint8_t> DK; DK.reserve(dkLen);

        for (std::size_t i = 1; i <= l; ++i)
        {
            // Salt || INT_32_BE(i)
            std::vector<std::uint8_t> Si = salt;
            std::uint8_t beIndex[4] = {
                static_cast<std::uint8_t>((i >> 24) & 0xFF),
                static_cast<std::uint8_t>((i >> 16) & 0xFF),
                static_cast<std::uint8_t>((i >> 8) & 0xFF),
                static_cast<std::uint8_t>((i >> 0) & 0xFF),
            };
            Si.insert(Si.end(), beIndex, beIndex + 4);

            // U1
            auto U = HMAC_SHA256(P, Si);
            std::array<std::uint8_t, 32> T = U; // running XOR

            // U2..Uc
            for (std::uint32_t j = 2; j <= iterations; ++j)
            {
                std::vector<std::uint8_t> Uprev(U.begin(), U.end());
                U = HMAC_SHA256(P, Uprev);
                for (std::size_t k = 0; k < 32; ++k)
                    T[k] ^= U[k];
            }

            // Append to derived key
            std::size_t take = (i == l) ? r : hLen;
            DK.insert(DK.end(), T.begin(), T.begin() + take);
        }

        return DK;
    }
}
