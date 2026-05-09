#pragma once

class ServerSetting
{
private:
	std::wstring	m_ip;
	uint16			m_port;
	int32			m_maxSessionCount;

	std::string		m_dbHost;
	int32			m_dbPort;
	std::string		m_dbUser;
	std::string		m_dbPassword;
	std::string		m_dbName;
	int32			m_dbPoolSize;

	const char* GetEnvVar(const char* varName, const char* defaultValue);
	bool GetEnvVarUsingWinAPI(const char* varName, char* buffer, size_t bufferSize, const char* defaultValue);

public:
	ServerSetting();
	~ServerSetting();

	std::wstring	GetIp()					const { return m_ip; }
	uint16			GetPort()				const { return m_port; }
	int32			GetMaxSessionCount()	const { return m_maxSessionCount; }

	std::string		GetDbHost()				const { return m_dbHost; }
	int32			GetDbPort()				const { return m_dbPort; }
	std::string		GetDbUser()				const { return m_dbUser; }
	std::string		GetDbPassword()			const { return m_dbPassword; }
	std::string		GetDbName()				const { return m_dbName; }
	int32			GetDbPoolSize()			const { return m_dbPoolSize; }
};