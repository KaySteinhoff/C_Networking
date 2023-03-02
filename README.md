# C Networking system

This networking system wasn't build nor meant for large scale use. It uses an int32_t pointer to store and send data and I would advise you to look at another system if your goal is a large operation like a forum or similar as configuring the values of the array might be annoying at that point. 
Games might be challenging too, though I havn't used it.

Alright if you are still here let's get started.

## General information

The Headers are build as to not force the user to complie all files which might become more of an issue as this project goes on (I don't think two files are that resoucre heavy ;) )

- Networking
  - Server.h
  - Client.h

## Use a Server

To create and use a Server the Server.h file contains a structure that handles all the variables to make life as easy as possible.
The user still has to configure a few things though.

```
struct Server sampleServer;
sampleServer.opt = True;
// The standard message send to all joining clients
sampleServer.message = "Welcome to the Server!\n";
// Maximum number of clients
sampleServer.max_clients = 100;
// Buffer size of int32_t's possible to fill with data being send (actual number of bytes used are 4*N in this example 4096 or 4kb)
sampleServer.bufferSize = 1024;
```

The server is started when calling the SetupServer() function which generates all client sockets and creates all handles required.

```
SetupServer(&sampeleServer);
```

Is the Server up-and-running the function will return a non zero value. Else the start-up is stopped and said non zero value is returned;

```
if(SetupServer(&sampeleServer) != 0)
  return -1;
```

After the startup Clients can join, how ever to do so the UpdateServer() function has to be called.
I recommend using a simple infinite loop to achive the desired effect. 
UpdateServer() will too return a non zero value of 1 when a I/O operation is comming in(Data is being send, Client is disconnected)

```
while(1)
{
  if(UpdateServer(&sampleServer) == 1)
  {
    printf("%s\n", sampleServer.data);
  }
}
```

## Use a Client

A Client is build very similar to the Server.

```
struct Client sampleClient;
sampleClient.ip_address = "127.0.0.1";
sampleClient.port = 8000;
```

Starting is very similar too.

```
if(SetupClient(&sampleClient) != 0)
  return -1;
```

Sending and receiving data works via two functions Send() and Receive(). (Who would've thought)

```
// Receiving Welcome message from Server
Receive(&sampleClient);
printf("%s", sampleClient.data);
memcpy(sampleClient.data, "Hello back from Client", sampleClient.bufferSize);
Send(&sampleClient);
```

And that's basically it! Values can too be send using the functions. I would suggest adding them at the back of the buffer in order to not conflict with any text you might add the the array as memcpy for example will always add it starting from the beginning.

```
// In this example sampleClient.bufferSize is 32 
memcpy(sampleClient.data, "Player1");
sampleClient.data[30] = playerX;
sampleClient.data[31] = playerY;
Send(&sampleClient);
```

A simple example program is also located in the repository under C_Networking/examples.

# Have fun!
