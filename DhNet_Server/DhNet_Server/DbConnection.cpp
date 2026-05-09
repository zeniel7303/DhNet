#include "stdafx.h"
#include "DbConnection.h"
#include "DbConnectionPool.h"

DbConnection::DbConnection(DbConnectionPool* pool)
    : _pool(pool), _conn(pool ? pool->Acquire() : nullptr)
{
}

DbConnection::~DbConnection()
{
    if (_pool && _conn)
        _pool->Release(_conn);
}
