#include "stdafx.h"
#include "DbConnectionPool.h"

bool DbConnectionPool::Init(const std::string& host, int32 port, const std::string& user,
                             const std::string& password, const std::string& dbName, int32 poolSize)
{
    WRITE_LOCK;

    for (int32 i = 0; i < poolSize; i++)
    {
        MYSQL* conn = mysql_init(nullptr);
        if (!conn)
        {
            std::cout << "[DbConnectionPool] mysql_init failed for connection " << i << "\n";
            continue;
        }

        bool connected = false;
        for (int32 retry = 0; retry < 3; retry++)
        {
            if (mysql_real_connect(conn, host.c_str(), user.c_str(),
                                   password.c_str(), dbName.c_str(),
                                   static_cast<unsigned int>(port),
                                   nullptr, 0))
            {
                connected = true;
                break;
            }
            std::cout << "[DbConnectionPool] Connect attempt " << (retry + 1)
                      << " failed: " << mysql_error(conn) << "\n";
            if (retry < 2)
                ::Sleep(200);
        }

        if (!connected)
        {
            std::cout << "[DbConnectionPool] Warning: connection " << i << " unavailable, skipped\n";
            mysql_close(conn);
            continue;
        }

        mysql_set_character_set(conn, "utf8mb4");
        _connections.push(conn);
        _poolSize++;
    }

    if (_poolSize > 0)
        std::cout << "[DbConnectionPool] Initialized with " << _poolSize << " connections\n";
    else
        std::cout << "[DbConnectionPool] Warning: no DB connections established\n";

    return _poolSize > 0;
}

void DbConnectionPool::Shutdown()
{
    WRITE_LOCK;
    while (!_connections.empty())
    {
        mysql_close(_connections.front());
        _connections.pop();
    }
    _poolSize = 0;
}

MYSQL* DbConnectionPool::Acquire()
{
    WRITE_LOCK;
    if (_connections.empty())
    {
        std::cout << "[DbConnectionPool] Warning: pool exhausted, returning nullptr\n";
        return nullptr;
    }
    MYSQL* conn = _connections.front();
    _connections.pop();
    return conn;
}

void DbConnectionPool::Release(MYSQL* conn)
{
    if (!conn)
        return;
    WRITE_LOCK;
    _connections.push(conn);
}
