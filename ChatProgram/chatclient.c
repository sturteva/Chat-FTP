/*Author:Andrew Sturtevant
 *Description: This is a simple chat client that will be communicating with a
 *Python written server code. */

/*The bulk of the logic of this program is from Oregon State University's 
 * CS344 - Operating Systems class. Lecture 4.2, slides 21 & 22*/

/*Handling white-space with scanf found here: https://bit.ly/2PoV3Te*/

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

int sendMessage(char handle[],int socketFD){

	int charsWritten;
	char buffer[512];
	char message[500];
	
	memset(buffer, '\0',sizeof(buffer));
	memset(message, '\0',sizeof(message));

	printf("%s",handle);
	//clear any previous stdin buffer
	scanf("%[^\n]s",message);
	while((getchar() != '\n'));//clear the stdin buffer
	message[strcspn(message,"\n")] = '\0';

	//Places the handle and message into a single buffer
	snprintf(buffer,sizeof(buffer), "%s%s",handle,message);
	
	char * quit = "\\quit";

	if(strcmp(message,quit) == 0) 
		return 0;

	charsWritten = send(socketFD,buffer,strlen(buffer),0);

	if(charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if(charsWritten < strlen(buffer))
		printf("CLIENT: WARNING: Not all data written to socket!\n");	
	

	return 1; 
		
}

int receiveMessage(int socketFD){

	int charsRead;
	char buffer[512];

	memset(buffer, '\0', sizeof(buffer));
	charsRead = recv(socketFD, buffer,sizeof(buffer)-1,0);

	if (charsRead == 0)
		return 0;
	printf("%s\n",buffer);
	return 1;
}

int main(int argc, char *argv[])
{

	int socketFD,portNumber, charsWritten, charsRead;
	char handle[11];
	char message[500];

	if(argc <3){
		fprintf(stderr, "USAGE: %s hostname port\n", argv[0]); 
		exit(0);
	}

	portNumber = atoi(argv[2]); //Get the port number, convert to in
	char* hostname = argv[1];
	socketFD = initiateContact(portNumber,hostname);

	printf("Please enter your handle(No more than 9 characters): ");
	memset(handle, '\0',sizeof(handle));

	scanf("%[^\n]s",handle);
	while((getchar() != '\n')); //Flush stdin
	handle[strcspn(handle, "\n")] = '>'; //Removes trailing \n

	int sendGoing = 1;
	int recvGoing = 1;
	while(sendGoing && recvGoing){
		sendGoing = sendMessage(handle,socketFD);

		if(sendGoing != 1)
			break;
		recvGoing = receiveMessage(socketFD);
		
	}

	if(recvGoing == 0){
		printf("Server Disconnected\n");
	}
	close(socketFD);

	return 0;
}
