/*Author:Andrew Sturtevant
 *Description: This is a simple chat client that will be communicating with a
 *Python written server code. */

/*The bulk of the logic of this program is from Oregon State University's 
 * CS344 - Operating Systems class. Lecture 4.2, slides 21 & 22*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(const char *msg) {perror(msg); exit(0);} //Error function

int initiateContact(int portNumber,char* hostname){

	int socketFD;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;

	//Set up server address struct
	memset((char*) &serverAddress, '\0',sizeof(serverAddress));

	serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(portNumber);
        serverHostInfo = gethostbyname(hostname);
        if(serverHostInfo == NULL){
                fprintf(stderr, "CLIENT: ERROR, no such host\n");
                exit(0);
        }

        memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length);

        //Set up the socket
        socketFD = socket(AF_INET,SOCK_STREAM,0);
                if(socketFD < 0) error("CLIENT:ERROR opening socket");


        //Connect to Server
        if(connect(socketFD,(struct sockaddr*)&serverAddress,sizeof(serverAddress)) < 0)
                error("CLIENT: ERROR connecting");
        
	return socketFD;	
}

int main(int argc, char *argv[])
{

	int socketFD,portNumber, charsWritten, charsRead;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char buffer[512];

	if(argc <3){
		fprintf(stderr, "USAGE: %s hostname port\n", argv[0]); 
		exit(0);
	}
/*
	//Set up server address struct
	memset((char*) &serverAddress, '\0', sizeof(serverAddress));

	portNumber = atoi(argv[2]); //Get the port number, convert to int

	serverAddress.sin_family = AF_INET; 
	serverAddress.sin_port = htons(portNumber);
	serverHostInfo = gethostbyname(argv[1]);
	if(serverHostInfo == NULL){
		fprintf(stderr, "CLIENT: ERROR, no such host\n");
		exit(0);
	}

	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length);

	//Set up the socket
	socketFD = socket(AF_INET,SOCK_STREAM,0);
	if(socketFD < 0) error("CLIENT:ERROR opening socket");
	
	//Connect to Server
	if(connect(socketFD,(struct sockaddr*)&serverAddress, 
		sizeof(serverAddress)) < 0)
		error("CLIENT: ERROR connecting");
*/
	portNumber = atoi(argv[2]); //Get the port number, convert to in
	char* hostname = argv[1];
	socketFD = initiateContact(portNumber,hostname);

	//Get input message from user
	printf("ENTER MESSAGE, press ENTER");
	memset(buffer, '\0',sizeof(buffer));
	fgets(buffer, sizeof(buffer) -1, stdin);
	buffer[strcspn(buffer, "\n")] = '\0'; // Remove the trailing \n that fgets adds	

	//Send message to server
	charsWritten = send(socketFD,buffer, strlen(buffer), 0);
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");

	//NOTE TO ANDREW: CHECK CS344 programing assignemtn, probably
	//fixed this particular problem
	if(charsWritten < strlen(buffer))
		printf("CLIENT: WARNING: Not all data written to socket!\n");

	//Get return message from server
	memset(buffer, '\0', sizeof(buffer)); //clear out buffer
	charsRead = recv(socketFD, buffer,sizeof(buffer)-1,0);
	if(charsRead <0) error("CLIENT:ERROR reading from socket");
	printf("CLIENT: MSG I GOT: %s\n",buffer);
	close(socketFD);

	return 0;
}
