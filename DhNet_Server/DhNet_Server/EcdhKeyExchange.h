#pragma once
#include <openssl/evp.h>
#include <openssl/ec.h>
#include <openssl/core_names.h>
#include <openssl/param_build.h>

class EcdhKeyExchange
{
public:
    // Returns a new P-256 ephemeral key pair. Caller must EVP_PKEY_free.
    static EVP_PKEY* GenerateKeyPair()
    {
        return EVP_EC_gen("P-256");
    }

    // Exports the uncompressed public point (0x04 | X | Y = 65 bytes).
    static bool GetPublicKeyBytes(EVP_PKEY* pkey, uint8* out65)
    {
        size_t len = 65;
        return EVP_PKEY_get_octet_string_param(pkey, OSSL_PKEY_PARAM_PUB_KEY,
                                               out65, 65, &len) == 1 && len == 65;
    }

    // Performs ECDH and writes the raw 32-byte shared secret to secret32.
    static bool DeriveSharedSecret(EVP_PKEY* myPriv, const uint8* peerPub65, uint8* secret32)
    {
        EVP_PKEY* peerKey = ImportPeerKey(peerPub65);
        if (!peerKey) return false;

        EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(myPriv, nullptr);
        bool ok = false;
        if (ctx && EVP_PKEY_derive_init(ctx) > 0
                && EVP_PKEY_derive_set_peer(ctx, peerKey) > 0)
        {
            size_t len = 32;
            ok = EVP_PKEY_derive(ctx, secret32, &len) > 0 && len == 32;
        }
        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(peerKey);
        return ok;
    }

    // SHA-256(label || secret32) → first 16 bytes as AES-128 session key.
    // Returns false if any OpenSSL operation fails; keyOut16 is zeroed on failure.
    static bool DeriveSessionKey(const uint8* secret32, uint8* keyOut16)
    {
        static const char label[] = "DhNet-session-key";

        uint8        hash[32] = {};
        unsigned int hashLen  = 32;

        EVP_MD_CTX* ctx = EVP_MD_CTX_new();
        if (!ctx) { memset(keyOut16, 0, 16); return false; }

        bool ok = EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr) == 1
               && EVP_DigestUpdate(ctx, label, sizeof(label) - 1) == 1
               && EVP_DigestUpdate(ctx, secret32, 32) == 1
               && EVP_DigestFinal_ex(ctx, hash, &hashLen) == 1
               && hashLen == 32;

        EVP_MD_CTX_free(ctx);

        if (ok)
            memcpy(keyOut16, hash, 16);
        else
            memset(keyOut16, 0, 16);

        return ok;
    }

private:
    static EVP_PKEY* ImportPeerKey(const uint8* pub65)
    {
        OSSL_PARAM_BLD* bld = OSSL_PARAM_BLD_new();
        if (!bld) return nullptr;

        OSSL_PARAM_BLD_push_utf8_string(bld, OSSL_PKEY_PARAM_GROUP_NAME, "prime256v1", 0);
        OSSL_PARAM_BLD_push_octet_string(bld, OSSL_PKEY_PARAM_PUB_KEY, pub65, 65);
        OSSL_PARAM* params = OSSL_PARAM_BLD_to_param(bld);
        OSSL_PARAM_BLD_free(bld);
        if (!params) return nullptr;

        EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_from_name(nullptr, "EC", nullptr);
        EVP_PKEY*     peerKey = nullptr;
        if (ctx && EVP_PKEY_fromdata_init(ctx) > 0)
            EVP_PKEY_fromdata(ctx, &peerKey, EVP_PKEY_PUBLIC_KEY, params);
        OSSL_PARAM_free(params);
        EVP_PKEY_CTX_free(ctx);
        return peerKey;
    }
};
