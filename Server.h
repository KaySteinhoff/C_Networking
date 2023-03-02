#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/time.h>

#define TRUE 1
#define FALSE 0
#define PORT 8000

struct Server
{
	int opt;
	int master_socket, addrlen, new_socket, max_clients, activity, valread, sd;
	int* client_socket;
	int max_sd;
	struct sockaddr_in address;
	
	char* message;
	fd_set readfds;
	int bufferSize;
	int32_t* data;
	char* buffer;
};

int SetupServer(struct Server* server)
{
	server->client_socket = (int*)malloc(server->max_clients * sizeof(int));
	
	for(int i = 0; i < server->max_clients; ++i)
	{
		server->client_socket[i] = 0;
	}
	
	if((server->master_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("ERROR_SETUP : Failure to create Server.");
		return EXIT_FAILURE;
	}

	if(setsockopt(server->master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&server->opt, sizeof(server->opt)) < 0)
	{
		perror("ERROR_INIT : Failure to initialize master socket.");
		return EXIT_FAILURE;
	}

	server->address.sin_family = AF_INET;
	server->address.sin_addr.s_addr = INADDR_ANY;
	server->address.sin_port = htons(PORT);

	if(bind(server->master_socket, (struct sockaddr*)&server->address, sizeof(server->address)) < 0)
	{
		perror("ERROR_PORT : Failed to bind master socket to port 8000.");
		return EXIT_FAILURE;
	}

	printf("Master socket now listening on port %d.\n", PORT);

	if(listen(server->master_socket, 3) < 0)
	{
		perror("ERROR_LISTEN : Failed to listen to socket.");
		return EXIT_FAILURE;
	}

	server->addrlen = sizeof(server->address);
	
	server->data = (int32_t*)malloc(server->bufferSize*sizeof(int32_t));
	server->buffer = (char*)server->data;
		
	return 0;
}

int UpdateServer(struct Server* server)
{
	int result = 0;
	FD_ZERO(&server->readfds);

	FD_SET(server->master_socket, &server->readfds);
	server->max_sd = server->master_socket;

	for(int i = 0; i < server->max_clients; ++i)
	{
		server->sd = server->client_socket[i];

		if(server->sd > 0)
			FD_SET(server->sd, &server->readfds);

		if(server->sd > server->max_sd)
			server->max_sd = server->sd;
	}

	server->activity = select(server->max_sd + 1, &server->readfds, NULL, NULL, NULL);

	if((server->activity < 0) && (errno != EINTR))
	{
		printf("Selection error!");
	}

	//Incoming Connection
	if(FD_ISSET(server->master_socket, &server->readfds))
	{
		if((server->new_socket = accept(server->master_socket, (struct sockaddr*)&server->address, (socklen_t*)&server->addrlen)) < 0)
		{
			perror("ERROR_ACCEPT : Failure to accept client.");
			exit(EXIT_FAILURE);
		}

		printf("New connection, socket fd is %d, ip is %s, port : %d\n", server->new_socket, inet_ntoa(server->address.sin_addr), ntohs(server->address.sin_port));

		if(write(server->new_socket, server->message, strlen(server->message)) != strlen(server->message))
		{
			perror("ERROR_SEND : Failure to send data!");
		}

		puts("Welcome send successfully!");

		for(int i = 0; i < server->max_clients; ++i)
		{
			if(server->client_socket[i] == 0)
			{
				server->client_socket[i] = server->new_socket;
				printf("Adding to list of sockets as %d\n", i);
				break;
			}
		}
	}

	//IO operation from client
	for(int i = 0; i < server->max_clients; ++i)
	{
		server->sd = server->client_socket[i];

		if(FD_ISSET(server->sd, &server->readfds))
		{
			if((server->valread = read(server->sd, server->buffer, server->bufferSize)) == 0)
			{
				getpeername(server->sd, (struct sockaddr*)&server->address, (socklen_t*)&server->addrlen);
				printf("Host disconnected, ip %s, port %d\n", inet_ntoa(server->address.sin_addr), ntohs(server->address.sin_port));
				close(server->sd);
				server->client_socket[i] = 0;
			}else{
				write(server->sd, server->buffer, server->bufferSize);
				result = 1;
			}
		}
	}
	return result;
}

