#include "stdafx.h"
#include "ServerSetting.h"

ServerSetting::ServerSetting()
{
    char buffer[256] = {0};
    GetEnvVarUsingWinAPI("DhNet_IP", buffer, sizeof(buffer), "127.0.0.1");
    m_ip = std::wstring(buffer, buffer + strlen(buffer));

    GetEnvVarUsingWinAPI("DhNet_PORT", buffer, sizeof(buffer), "7900");
    m_port = static_cast<uint16>(std::stoi(buffer));

    GetEnvVarUsingWinAPI("DhNet_MAX_SESSION_COUNT", buffer, sizeof(buffer), "1000");
    m_maxSessionCount = std::stoi(buffer);

    GetEnvVarUsingWinAPI("DhNet_DB_HOST", buffer, sizeof(buffer), "127.0.0.1");
    m_dbHost = buffer;

    GetEnvVarUsingWinAPI("DhNet_DB_PORT", buffer, sizeof(buffer), "3306");
    m_dbPort = std::stoi(buffer);

    GetEnvVarUsingWinAPI("DhNet_DB_USER", buffer, sizeof(buffer), "dhnet");
    m_dbUser = buffer;

    GetEnvVarUsingWinAPI("DhNet_DB_PASSWORD", buffer, sizeof(buffer), "dhnet_pw");
    m_dbPassword = buffer;

    GetEnvVarUsingWinAPI("DhNet_DB_NAME", buffer, sizeof(buffer), "dhnet_db");
    m_dbName = buffer;

    GetEnvVarUsingWinAPI("DhNet_DB_POOL_SIZE", buffer, sizeof(buffer), "4");
    m_dbPoolSize = std::stoi(buffer);

    GetEnvVarUsingWinAPI("DhNet_GRPC_ADDRESS", buffer, sizeof(buffer), "127.0.0.1:7820");
    m_grpcAddress = buffer;
}

bool ServerSetting::GetEnvVarUsingWinAPI(const char* varName, char* buffer, size_t bufferSize, const char* defaultValue)
{
    memset(buffer, 0, bufferSize);

    DWORD result = GetEnvironmentVariableA(varName, buffer, static_cast<DWORD>(bufferSize));
    if (result > 0 && result < bufferSize)
    {
        return true;
    }
    else
    {
        strncpy_s(buffer, bufferSize, defaultValue, _TRUNCATE);
        buffer[bufferSize - 1] = '\0';
        return false;
    }
}

ServerSetting::~ServerSetting()
{
}
