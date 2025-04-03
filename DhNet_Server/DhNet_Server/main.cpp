#include "stdafx.h"
#include "ServerSetting.h"
#include "GameServer.h"

static void CheckAllEnv(char** envp)
{
#if _DEBUG
	for (char** env = envp; *env != 0; env++)
	{
		char* thisEnv = *env;
		printf("%s\n", thisEnv);
	}
#endif
}

int main(int argc, char** argv, char** envp)
{
	// CheckAllEnv(envp);

	std::shared_ptr<ServerSetting> serverSetting = std::make_shared<ServerSetting>();

	std::shared_ptr<GameServer> gameServer = std::make_shared<GameServer>(serverSetting);

	gameServer->StartServer();

	return 0;
}