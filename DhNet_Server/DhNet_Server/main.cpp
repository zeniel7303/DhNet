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

	shared_ptr<ServerSetting> serverSetting = make_shared<ServerSetting>();

	shared_ptr<GameServer> gameServer = make_shared<GameServer>(serverSetting);

	gameServer->StartServer();

	return 0;
}