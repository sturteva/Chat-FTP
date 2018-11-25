Ensure that you have both chatclient.c, chatserve and the makefile in the same directory.

The following are commands in the command line

Step 1: make all

TO RUN THE SERVER:

Step 2: ./ftserve [PORT NUMBER]

To RUN THE CLIENT:

Step 2: ./ftclient [SERVER HOST NAME] [CONTROL PORT NUMBER] [COMMAND] /*If -g [FILENAME]*/ [DATAPORT NUMBER]

To remove executable and revert permission of ftclient do:

make clean

INSTRUCTIONS:

This FTP program will all you to list the directory that the server is in with the command: -l

It will also allow you to transfer TEXT files to the client as well with the command: -g

	If the command '-g' is given, the FILENAME will follow directly after the command and then the data port # you wish to use.

