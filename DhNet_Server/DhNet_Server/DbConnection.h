#pragma once

class DbConnectionPool;

class DbConnection
{
	DbConnectionPool*	_pool;
	MYSQL*				_conn;

public:
	explicit DbConnection(DbConnectionPool* pool);
	~DbConnection();

	MYSQL*	Get() const		{ return _conn; }
	bool	IsValid() const	{ return _conn != nullptr; }

	DbConnection(const DbConnection&) = delete;
	DbConnection& operator=(const DbConnection&) = delete;
};
