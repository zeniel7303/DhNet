#include "stdafx.h"
#include "AccountRepository.h"
#include "CryptoUtil.h"

// Timing-safe reject: run PBKDF2 even when username is not found.
// Initialized once at startup with a random salt via RAND_bytes so the dummy run
// cannot be distinguished from a real verification path by cache/timing analysis.
namespace {
    struct DummyInit {
        std::string hash, salt;
        DummyInit()
        {
            bool ok = CryptoUtil::HashPassword("__dummy_password_init__", hash, salt);
            ASSERT_CRASH(ok); // OpenSSL RAND_bytes failure at startup is unrecoverable
        }
    };
    static DummyInit g_dummy;
}

bool AccountRepository::Authenticate(MYSQL* conn, const std::string& username,
                                     const std::string& password,
                                     uint64& outId, std::string& outName)
{
    if (!conn)
        return false;

    MYSQL_STMT* stmt = mysql_stmt_init(conn);
    if (!stmt)
        return false;

    const char* query = "SELECT id, password_hash, salt FROM accounts WHERE username=? LIMIT 1";
    if (mysql_stmt_prepare(stmt, query, static_cast<unsigned long>(strlen(query))) != 0)
    {
        mysql_stmt_close(stmt);
        return false;
    }

    MYSQL_BIND in[1] = {};
    unsigned long usernameLen = static_cast<unsigned long>(username.size());
    in[0].buffer_type   = MYSQL_TYPE_STRING;
    in[0].buffer        = const_cast<char*>(username.c_str());
    in[0].buffer_length = static_cast<unsigned long>(username.size());
    in[0].length        = &usernameLen;

    if (mysql_stmt_bind_param(stmt, in) != 0)
    {
        mysql_stmt_close(stmt);
        return false;
    }

    if (mysql_stmt_execute(stmt) != 0)
    {
        mysql_stmt_close(stmt);
        return false;
    }

    uint64_t        id      = 0;
    char            hashBuf[65] = {};
    char            saltBuf[33] = {};
    unsigned long   hashLen = 0, saltLen = 0;

    MYSQL_BIND out[3] = {};
    out[0].buffer_type  = MYSQL_TYPE_LONGLONG;
    out[0].buffer       = &id;
    out[0].is_unsigned  = 1;

    out[1].buffer_type   = MYSQL_TYPE_STRING;
    out[1].buffer        = hashBuf;
    out[1].buffer_length = sizeof(hashBuf) - 1;
    out[1].length        = &hashLen;

    out[2].buffer_type   = MYSQL_TYPE_STRING;
    out[2].buffer        = saltBuf;
    out[2].buffer_length = sizeof(saltBuf) - 1;
    out[2].length        = &saltLen;

    if (mysql_stmt_bind_result(stmt, out) != 0)
    {
        mysql_stmt_close(stmt);
        return false;
    }

    mysql_stmt_store_result(stmt);

    int fetchResult = mysql_stmt_fetch(stmt);
    mysql_stmt_free_result(stmt);
    mysql_stmt_close(stmt);

    if (fetchResult != 0)
    {
        // Username not found — still run PBKDF2 to prevent timing-based user enumeration
        CryptoUtil::VerifyPassword(password, g_dummy.hash, g_dummy.salt);
        return false;
    }

    hashBuf[hashLen] = '\0';
    saltBuf[saltLen] = '\0';

    bool ok = CryptoUtil::VerifyPassword(password, hashBuf, saltBuf);
    if (ok)
    {
        outId   = static_cast<uint64>(id);
        outName = username;
    }
    return ok;
}
