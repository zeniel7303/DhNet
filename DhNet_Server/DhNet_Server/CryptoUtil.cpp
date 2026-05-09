#include "stdafx.h"
#include "CryptoUtil.h"

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/crypto.h>

static constexpr int PBKDF2_ITERATIONS	= 100000;
static constexpr int HASH_BYTES			= 32;	// 256 bits → 64 hex chars
static constexpr int SALT_BYTES			= 16;	// 128 bits → 32 hex chars

bool CryptoUtil::HashPassword(const std::string& password,
                              std::string& outHashHex, std::string& outSaltHex)
{
    unsigned char salt[SALT_BYTES];
    if (RAND_bytes(salt, SALT_BYTES) != 1)
        return false;

    unsigned char hash[HASH_BYTES];
    bool ok = PKCS5_PBKDF2_HMAC(password.c_str(), static_cast<int>(password.size()),
                                  salt, SALT_BYTES,
                                  PBKDF2_ITERATIONS, EVP_sha256(),
                                  HASH_BYTES, hash) == 1;
    if (ok)
    {
        outHashHex = BytesToHex(hash, HASH_BYTES);
        outSaltHex = BytesToHex(salt, SALT_BYTES);
    }

    OPENSSL_cleanse(hash, HASH_BYTES);
    OPENSSL_cleanse(salt, SALT_BYTES);
    return ok;
}

bool CryptoUtil::VerifyPassword(const std::string& password,
                                const std::string& storedHashHex,
                                const std::string& storedSaltHex)
{
    std::vector<unsigned char> salt;
    if (!HexToBytes(storedSaltHex, salt) || salt.size() != SALT_BYTES)
        return false;

    unsigned char computed[HASH_BYTES];
    bool pbkdf2ok = PKCS5_PBKDF2_HMAC(password.c_str(), static_cast<int>(password.size()),
                                        salt.data(), SALT_BYTES,
                                        PBKDF2_ITERATIONS, EVP_sha256(),
                                        HASH_BYTES, computed) == 1;

    OPENSSL_cleanse(salt.data(), salt.size());

    if (!pbkdf2ok)
    {
        OPENSSL_cleanse(computed, HASH_BYTES);
        return false;
    }

    std::vector<unsigned char> storedHash;
    if (!HexToBytes(storedHashHex, storedHash) || storedHash.size() != HASH_BYTES)
    {
        OPENSSL_cleanse(computed, HASH_BYTES);
        return false;
    }

    bool result = CRYPTO_memcmp(computed, storedHash.data(), HASH_BYTES) == 0;
    OPENSSL_cleanse(computed, HASH_BYTES);
    OPENSSL_cleanse(storedHash.data(), storedHash.size());
    return result;
}

std::string CryptoUtil::BytesToHex(const unsigned char* data, size_t len)
{
    static const char HEX_CHARS[] = "0123456789abcdef";
    std::string result(len * 2, '\0');
    for (size_t i = 0; i < len; i++)
    {
        result[i * 2]     = HEX_CHARS[(data[i] >> 4) & 0xF];
        result[i * 2 + 1] = HEX_CHARS[data[i] & 0xF];
    }
    return result;
}

bool CryptoUtil::HexToBytes(const std::string& hex, std::vector<unsigned char>& out)
{
    if (hex.size() % 2 != 0)
        return false;

    out.resize(hex.size() / 2);
    for (size_t i = 0; i < out.size(); i++)
    {
        auto hexVal = [](char c) -> int {
            if (c >= '0' && c <= '9') return c - '0';
            if (c >= 'a' && c <= 'f') return c - 'a' + 10;
            if (c >= 'A' && c <= 'F') return c - 'A' + 10;
            return -1;
        };
        int hi = hexVal(hex[i * 2]);
        int lo = hexVal(hex[i * 2 + 1]);
        if (hi < 0 || lo < 0) return false;
        out[i] = static_cast<unsigned char>((hi << 4) | lo);
    }
    return true;
}
