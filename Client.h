#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>

struct Client
{
	int status, valread, client_fd;
	struct sockaddr_in serv_addr;
	int connected;
	char* ip_address;
	int port;
	
	int bufferSize;
	int32_t* data;
	char* sendBuffer;
};

int SetupClient(struct Client* client)
{
	if((client->client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("ERROR_SETUP : Failure to create client");
		client->connected = 1;
		return -1;
	}

	client->serv_addr.sin_family = AF_INET;
	client->serv_addr.sin_port = htons(client->port);

	if(inet_pton(AF_INET, client->ip_address, &client->serv_addr.sin_addr) <= 0)
	{
		perror("ERROR_ADDRESS : Failure server '127.0.0.1' does not exsist");
		client->connected = 1;
		return -1;
	}

	if((client->status = connect(client->client_fd, (struct sockaddr*)&client->serv_addr, sizeof(client->serv_addr))) < 0)
	{
		perror("ERROR_CONNECT : Failure to connect to server '127.0.0.1' via port: 8000");
		client->connected = 1;
		return -1;
	}
		
	client->data = (int32_t*)malloc(client->bufferSize*sizeof(int32_t));
	//client->data = (char*)client->numData;
	
	return 0;
}

int Send(struct Client* client)
{
	//memset(client->sendBuffer, '\0', client->bufferSize);
	client->sendBuffer = (char*)client->data;
	if(write(client->client_fd, client->sendBuffer, client->bufferSize) != client->bufferSize)
		return -1;
	
	return 0;
}

int Receive(struct Client* client)
{
	client->sendBuffer = (char*)client->data;
	return read(client->client_fd, client->sendBuffer, client->bufferSize);
}
