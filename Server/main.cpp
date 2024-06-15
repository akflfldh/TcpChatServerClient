#include"Server.h"


int main()
{
	Server server;
	if (server.Initialize())
	{
		server.Run();
	}

	
	return 0;
}

