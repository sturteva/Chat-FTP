/*Author: Andrew Sturtevant
 * Description: This is a file transfer server.
*/

/*The bulk of the logic of this server program is from Oregon State University's
 * CS344 - Operating Systems Class. Lecture 4.3, slides 16 & 17, specificallly
 * the basic set up of the sockets*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues

int startUp(portNumber){
	struct sockaddr_in serverAddress;
	int listenSocketFD;

	memset((char *) &serverAddress, '\0', sizeof(serverAddress));//Clear out address struct
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); //Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; //Any address is allowed for connection

	/* Set up the socket*/
	listenSocketFD = socket(AF_INET,SOCK_STREAM,0); //Creates the socket
	if (listenSocketFD < 0) error("ERROR opening socket");

	/*Enable the socket to begin listening*/
	if(bind(listenSocketFD,(struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)error("ERROR on binding");

	printf("Server open on %d\n", portNumber);
	return listenSocketFD;
}

void handleRequest(establishedConnectionFD){
	
	char buffer[1024];
	int charsRead;
	
	memset(buffer, '\0',sizeof(buffer));
	charsRead = recv(establishedConnectionFD, buffer, sizeof(buffer)-1,0);

	if(charsRead < 0) error("ERROR reading from socket");

	//Test line
	printf("SERVER from client: \"%s\"\n", buffer);

	char* token = strtok(buffer," " );
	printf("BOO\n");
	printf("token: %s\n",token);
	if(strcmp(token,"-g") != 0 && strcmp(token,"-l") != 0){
		printf("Invalid command sent\n");
	}

	else if(strcmp(token,"-l") == 0){
		printf("Listing Command Received");
	}

	else if(strcmp(token,"-g") == 0){
		printf("Get command Received");
	}
}
int main(int argc, char *argv[])
{
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
		socklen_t sizeOfClientInfo;
	char buffer[256];
	struct sockaddr_in serverAddress, clientAddress;

	/*START OF THE SERVER START-UP*/
	if (argc < 2) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); } // Check usage & args


	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	listenSocketFD = startUp(portNumber);
	listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections
	
	/* Accept a connection, blocking if one is not available until one connects*/
	while(1){
	sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect

	establishedConnectionFD = accept(listenSocketFD,(struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept

	if (establishedConnectionFD < 0) error("ERROR on accept");

	char host[1024];
	char service[20];

	getnameinfo(&clientAddress,sizeof(clientAddress),host,sizeof(host),service,sizeof(service),0); 
	printf("Connection from %s\n",host);
	handleRequest(establishedConnectionFD);

	/* Send a Success message back to the client*/
	charsRead = send(establishedConnectionFD, "I am the server, and I got your message", 39, 0); // Send success back
	if (charsRead < 0) error("ERROR writing to socket");

	close(establishedConnectionFD); // Close the existing socket which is connected to the client
	}

	close(listenSocketFD); // Close the listening socket

	return 0;
}
