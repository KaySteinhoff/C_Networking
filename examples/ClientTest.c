#include "Networking.h"

int main(int argc, char const* argv[])
{		
	struct Client myClient;
	myClient.ip_address = "127.0.0.1";
	myClient.port = 8000;
	if(SetupClient(&myClient) < 0)
		printf("Cloudn't connect!");
	printf("Created Server!\n");
	
	Receive(&myClient);
	printf("%s", myClient.data);
	
	memcpy(myClient.data, "Test", myClient.bufferSize);
	myClient.data[4] = 15;
	myClient.data[5] = 25;
	Send(&myClient);
	
	printf("Send message!\n");
}