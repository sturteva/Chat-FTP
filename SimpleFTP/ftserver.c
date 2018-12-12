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
#include <netdb.h>
#include <dirent.h> //To do stuff with the directory

void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues

/*This sets up the server, it needs the portNumber that was a command line 
 * argument.It is going to return the socket that it sets up to list*/
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

/*Pulled from Project1 for CS372*/
/*This function will set up a client-side socket, with the "server's" hostname
 * This is to set up the dataPort when it is needed
 * It returns the socket that it sets up as a client socket*/
int initiateContact(int portNumber, char* hostname){
	int socketFD;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;

	//Set up server address struct
	memset((char*) &serverAddress, '\0',sizeof(serverAddress));

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(portNumber);
        serverHostInfo = gethostbyname(hostname);
        if(serverHostInfo == NULL){
		fprintf(stderr, "Server: ERROR, no such host\n");
                exit(0);
       }

       memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length);

	//Set up the socket
	socketFD = socket(AF_INET,SOCK_STREAM,0);
	if(socketFD < 0) error("Server:ERROR opening socket");
	//Connect to Server
	if(connect(socketFD,(struct sockaddr*)&serverAddress,sizeof(serverAddress)) < 0)
	error("Server: ERROR connecting");
	                                                                                                       
		return socketFD;

}

/*This function does a lot of the heavy lifting of handling the specific
 * request of the client.  In the future, it can probably be refactored to 
 * basically just determine the TYPE of request and than call a function to 
 * handle the specific request.  
 *
 * RIght now though, it needs client's hostname, the connectionPort# and the 
 * actual socket that has started the connection with the client 
 * (establisedConnectionFD)*/
void handleRequest(int establishedConnectionFD,int connectionPort, char client[]){
	
	char commandBuffer[1024];
	char dirBuffer[70000];
	int charsRead;
	int dataPort;
	int dataFD;
	DIR *d;
	struct dirent *dir;
	

	memset(dirBuffer,'\0',sizeof(dirBuffer));
	memset(commandBuffer, '\0',sizeof(commandBuffer));
	charsRead = recv(establishedConnectionFD, commandBuffer, sizeof(commandBuffer)-1,0);

	if(charsRead < 0) error("ERROR reading from socket");

	char* token = strtok(commandBuffer," " );

	/*This is what happens when the list command is received*/
	if(strcmp(token,"-l") == 0){
		dataPort = atoi(strtok(NULL," "));
		printf("List directory requested on port %d.\n",dataPort);
		dataFD = initiateContact(dataPort,(char*)client);

		d = opendir(".");

		/*Goes through the directory, writing all of the names into 
 		*directory buffer*/ 
		if(d){
			while((dir = readdir(d)) != NULL){
				strcat(dirBuffer,dir->d_name);
				strcat(dirBuffer,"\n");	
			}
			closedir(d);
		}
		int charsWritten = 0;

		/*This sends the directory to the client*/
		while(charsWritten < strlen(dirBuffer)){
			charsWritten+= send(dataFD,dirBuffer,strlen(dirBuffer),0);

			if(charsWritten < 0)
				error("SERVER: Error Sending Directory");
		}	
		
			
	}

	/*Get Command received*/
	else if(strcmp(token,"-g") == 0){
		dataPort = atoi(strtok(NULL," "));
		char* fileName = strtok(NULL," ");
		printf("File \"%s\" requested on port %d.\n",fileName,dataPort);

		dataFD = initiateContact(dataPort,(char*)client);
		FILE *fp;
		fp = fopen(fileName,"r");

		/*If the file does not exist, send error to client*/
		if(fp == NULL){
			printf("File not found. Sending error message to %s:%d\n",client,connectionPort);
			char* errMsg = "FILE NOT FOUND";
			charsRead = 0;
			charsRead = send(establishedConnectionFD,errMsg,14 ,0);

			if(charsRead <0) error("Server: Error sending error msg");
		}

		/*Otherwise prepare to send file to client*/
		else{
			printf("Sending \"%s\" to %s:%d\n",fileName,client,dataPort);
			char* msg = "Receiving";
			charsRead = send(establishedConnectionFD,msg,9,0);

			if(charsRead < 0) error("Server: Error sending Receiving");
	
			char* buffer = malloc(10000000);//create buffer for file
			int buffercount = 0;
			char c;
			
			/*Read each character of file and add it to the buffer*/
			while(1){
				
				c = fgetc(fp);
			
				if(c == EOF)
					break;
				else{
				buffer[buffercount] = c;
				buffercount++;
				}
			}

			int charsWritten = 0;
			/*Send the buffer*/
			while(charsWritten < strlen(buffer)){
				charsWritten += send(dataFD,buffer,strlen(buffer),0);
	
				if(charsWritten <0) error("SERVER: Writing file to socket");
			}

		
			/*Send sentinel when file complete*/
			char* sentinel = "!!!@@@!!!";
			charsWritten = send(dataFD,sentinel,strlen(sentinel),0);
			if(charsWritten < 0) error("Server: End Sentinel");
			
			free(buffer); //Free the buffer to prevent memory leak
		}

		
	}

	close(dataFD); //Close the data port/socket as it is no longer needed
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
	handleRequest(establishedConnectionFD,portNumber,host);

	close(establishedConnectionFD); // Close the existing socket which is connected to the client
	}

	close(listenSocketFD); // Close the listening socket

	return 0;
}
