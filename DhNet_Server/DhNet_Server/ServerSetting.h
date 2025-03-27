#pragma once

class ServerSetting
{
private:
	wstring		m_ip;
	uint16		m_port;
	int32		m_maxSessionCount;

	const char* GetEnvVar(const char* varName, const char* defaultValue);
	bool GetEnvVarUsingWinAPI(const char* varName, char* buffer, size_t bufferSize, const char* defaultValue);

public:
	ServerSetting();
	~ServerSetting();

	wstring		GetIp() const { return m_ip; }
	uint16		GetPort() const { return m_port; }
	int32		GetMaxSessionCount() const { return m_maxSessionCount; }
};