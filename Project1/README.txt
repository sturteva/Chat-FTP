Ensure that you have both chatclient.c, chatserve and the makefile in the same directory.

The following are commands in the command line

Step 1: make all

TO RUN THE SERVER:

Step 2: ./chatserve [PORT NUMBER]

To RUN THE CLIENT:

Step 2: ./chatclient [HOST NAME] [PORT NUMBER]

To remove executable and revert permission of chatserve do:

make clean

CHAT INSTRUCTIONS:

Upon start-up the server will ask for its handle. Client will as well.

You may send up to 500 character messages between the client and the server.

When you are ready to quit put in the command: "\quit".

If you are on the server side, it will disconnect the Client and you will continue listening until an interrupt signal is sent to the Server.

If you are on the client side, you will simply disconnect from the server. When the Client disconnects, the Server will revert to listening for another connection. 


