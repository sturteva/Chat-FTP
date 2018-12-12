Ensure that you have both chatclient.c, chatserve and the makefile in the same directory. As of right now, the client only takes two commands "-l" (Which lists the contents of the directory that the Server is on.  

The following are commands in the command line

Step 1: make all

NOTE: You can now move ftserver to which ever directory you would like it to live on. 

TO RUN THE SERVER:

Step 2: ./ftserver [CONTROL PORT NUMBER]

To RUN THE CLIENT:

Step 2: ./ftclient [SERVER HOST NAME] [CONTROL PORT NUMBER] [COMMAND] /*If -g [FILENAME]*/ [DATAPORT NUMBER]




To remove executable and revert permission of ftclient do:

make clean

INSTRUCTIONS:

This FTP program will allow you to list the directory that the server is in with the command: -l

It will also allow you to transfer TEXT files to the client as well with the command: -g

	If the command '-g' is given, the FILENAME will follow directly after the command and then the data port # you wish to use.

