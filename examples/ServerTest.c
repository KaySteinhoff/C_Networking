#include "Networking.h"

int main(int argc, char const* argv[])
{
	struct Server myServer;
	myServer.opt = TRUE;
	myServer.message = "Welcome from Server\n";
	myServer.max_clients = 100;
	myServer.bufferSize = 1024;
	if(SetupServer(&myServer) != 0)
		return -1;

	while(1)
	{
		if(UpdateServer(&myServer) == 1)
		{
			printf("%s\n", myServer.data);
			printf("%d\n", myServer.data[4]);
			printf("%d\n", myServer.data[5]);
		}
	}
	
	return 0;
}