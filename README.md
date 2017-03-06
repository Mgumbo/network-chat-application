# network-chat-application
NOTE: These programs were designed to run on a Linux-/Unix-based system.
Execution and output of this program may not work as intended if it is run
outside of a Linux-/Unix-based system.


PROGRAM DESCRIPTION
This set of programs comprises an application that establishes a TCP server,
allowing multiple clients to connect to the server. The clients that connect
can send messages to the server, which the server will transmit to all other
clients.


PROGRAMS/CONTENTS
This set of files for a client-server application includes the following:

	resources.h	
	server.c	
	client.c	
	makefile

Excluding `makefile`, all of the preceding files are written in C.

COMPILING AND EXECUTING THE FILES
--Compiling--
A makefile is included and is the simplest way to compile these files. The
following command will automatically compile all necessary files in addition
to automatically executing the server:

	>make

Alternatively, you can compile the programs manually using the following set of
commands, in order:

	>gcc resources.h
	>gcc -o server server.c
	>gcc -o client client.c

This series of commands will properly compile all necessary files if executed
correctly in order.

After the program is terminated, entering the command:

	>make clean

will remove any additional files produced by compilation.


--Executing--
Executing the server and client simultaneously will require at least 2 open
terminals. After compiling the files, the server and client can be executed
with the following commands, respectively:

	>./server
	>./client [IP address of server] [desired display name]

An example execution of the client could be:

	>./client 127.0.0.1 user_number_1
		  AND/OR
	>./client 127.0.0.1 user_number_2

Multiple clients will require multiple terminals to be open.

If the files were compiled using `make`, then the server will run automatically
and only the clients will require manual execution. Note that the server must be
running before any clients will be able to connect.

Both server and client applications can be terminated by typing `CTRL+c` at any
time during execution. The server will be notified when any client disconnects,
but other clients WILL NOT be notified.


--Usage--
When the server is running, any input to the server will produce no results and
will not be logged in the server log.

When running the client application, after connecting to the server, the client
will display a short confirmation of connection. Afterward, the user may type
anything they choose into the client application. Messages sent from one client
will be visible to all other clients, along with the display name of the sender.
For example (the arrows are for visualization and will not be displayed):

	CLIENT1 DISPLAY(user_number_1)				CLIENT2 DISPLAY(user_number_2)
	hello						        ------------>	user_number_1: hello
	user_number_2: hello		<------------	hello
	this program rocks!			------------>	user_number_1: this program rocks!
	user_number_2: me too		<------------	me too

A client's own username will not be displayed to themselves when they send a
message. Client's can also enter certain commands that will not be transmitted
to other clients. Commands begin with a forward slash (/) followed by specific
text. The current commands are available:

	COMMAND					    RESULT
	/quit OR /q				  Disconnect and exit
	/disconnect OR /dc	Disconnect and exit
	/exit OR /ex			  Disconnect and exit

Currently only disconnecting commands are available. More commands may be added
at a later time. These commands are not sent to other clients UNLESS the user
types a command incorrectly.
