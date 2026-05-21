#pragma once
#include <openssl/evp.h>
#include <openssl/rand.h>

class AesGcm
{
    uint8 m_key[16];

public:
    void Init(const uint8* key16) { memcpy(m_key, key16, 16); }
    ~AesGcm() { SecureZeroMemory(m_key, sizeof(m_key)); }

    // out = nonce(12) | ciphertext(plainLen) | tag(16), outLen = plainLen + 28
    bool Encrypt(const uint8* plain, int32 plainLen, uint8* out, int32& outLen) const
    {
        RAND_bytes(out, 12);

        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        if (!ctx) return false;

        int len = 0;
        bool ok = EVP_EncryptInit_ex(ctx, EVP_aes_128_gcm(), nullptr, m_key, out) == 1
               && EVP_EncryptUpdate(ctx, out + 12, &len, plain, plainLen) == 1
               && EVP_EncryptFinal_ex(ctx, out + 12 + len, &len) == 1
               && EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, out + 12 + plainLen) == 1;

        EVP_CIPHER_CTX_free(ctx);
        if (ok) outLen = 12 + plainLen + 16;
        return ok;
    }

    // in = nonce(12) | ciphertext | tag(16), outLen = inLen - 28
    bool Decrypt(const uint8* in, int32 inLen, uint8* out, int32& outLen) const
    {
        if (inLen < 28) return false;

        const uint8* nonce      = in;
        const uint8* ciphertext = in + 12;
        int32        cipherLen  = inLen - 28;
        const uint8* tag        = in + 12 + cipherLen;

        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        if (!ctx) return false;

        int  len = 0;
        bool ok  = EVP_DecryptInit_ex(ctx, EVP_aes_128_gcm(), nullptr, m_key, nonce) == 1
                && EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, 16, const_cast<uint8*>(tag)) == 1
                && EVP_DecryptUpdate(ctx, out, &len, ciphertext, cipherLen) == 1
                && EVP_DecryptFinal_ex(ctx, out + len, &len) > 0;

        EVP_CIPHER_CTX_free(ctx);
        if (ok) outLen = cipherLen;
        return ok;
    }
};
