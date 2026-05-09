#pragma once

class DbConnectionPool
{
	USE_LOCK;
	std::queue<MYSQL*>	_connections;
	int32				_poolSize = 0;

public:
	bool	Init(const std::string& host, int32 port, const std::string& user,
				 const std::string& password, const std::string& dbName, int32 poolSize);
	void	Shutdown();

	MYSQL*	Acquire();
	void	Release(MYSQL* conn);
	bool	IsReady() const { return _poolSize > 0; }
};
