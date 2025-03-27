#include "stdafx.h"
#include "ServerSetting.h"

ServerSetting::ServerSetting()
{
    char buffer[256] = {0};
    GetEnvVarUsingWinAPI("DhNet_IP", buffer, sizeof(buffer), "127.0.0.1");
    m_ip = wstring(buffer, buffer + strlen(buffer)); // Convert char* to wstring

    GetEnvVarUsingWinAPI("DhNet_PORT", buffer, sizeof(buffer), "7777");
    m_port = static_cast<uint16>(std::stoi(buffer));

    GetEnvVarUsingWinAPI("DhNet_MAX_SESSION_COUNT", buffer, sizeof(buffer), "1000");
    m_maxSessionCount = std::stoi(buffer);

    /*
    auto ip = GetEnvVar("DhNet_IP", "127.0.0.1");
    m_ip = wstring(ip, ip + strlen(ip)); // Convert char* to wstring
    m_port = static_cast<uint16>(std::stoi(GetEnvVar("DhNet_PORT", "7777")));
    m_maxSessionCount = std::stoi(GetEnvVar("DhNet_MAX_SESSION_COUNT", "1000"));
    */
}

const char* ServerSetting::GetEnvVar(const char* varName, const char* defaultValue)
{
    char* value;
    size_t valueSize;
    auto err = _dupenv_s(&value, &valueSize, varName);
    auto result = value ? value : defaultValue;
    if (value) free(value);
    return result;
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
        buffer[bufferSize - 1] = '\0'; // Ensure null-termination
        return false;
    }
}

ServerSetting::~ServerSetting()
{
}