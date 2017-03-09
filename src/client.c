/***********************************************************************
 * Author: Mgumbo
 *
 * Date Completed: 14 December 2016
 *
 * Description: A program that acts as a client interface for users to
 * connect to a server with. Once connected to a server, the client can
 * send messages to other clients also connected to the server.
 ***********************************************************************/
#include "resources.h"

// Function prototypes
void run_client(fd_set* cli_fds, fd_set* main_fd, int* max_fd, int* sock_fd, char* name);
void init_connection(char* ip_addr, fd_set* main_fd, fd_set* cli_fds, int* sock_fd, SA_in* serv_addr);
void send_msg(char* name, int sock_fd);
void recv_msg(int sock_fd);

/**
 * Runs the program.
 **/
int main(int argc, char** argv)
{
	// Local variables
	int sock_fd, max_fd; //fd for this client's connection and max fd value
	fd_set main_fd, cli_fds; //fd sets for main server and clients
	SA_in serv_addr; //address struct for server
	char* name; //username

	// Verify the correct arguments are used
	if (argc != 3)
	{
		printf("Proper usage: client <IP address> <username>\n");
		exit(1);
	} //end if

	// Set user's name
	name = argv[2];
	
	// Initialize connection to server
	init_connection(argv[1], &main_fd, &cli_fds, &sock_fd, &serv_addr);

	// Output successful connection	
	printf("Successfully connected to %s.\n", argv[1]);
	printf("You may begin typing at any time.\n\n");

	// Begin client interface
	run_client(&cli_fds, &main_fd, &max_fd, &sock_fd, name);

	// Close the connection
	close(sock_fd);
	return 0;
} //end main

/**
 * Begin running an infinite loop to process client activity, both from the
 * user and other clients. Sends a message if it's the user, and otherwise
 * receives a message if activity is from another client.
 **/
void run_client(fd_set* cli_fds, fd_set* main_fd, int* max_fd, int* sock_fd, char* name)
{
	*max_fd = *sock_fd;
	
	// Infinite loop to send and receive data
	int i; //loop control
	for (;;)
	{
		// Wait for activity from either you or another client
		*cli_fds = *main_fd;
		select(*max_fd+1, cli_fds, NULL, NULL, NULL);

		// Iterate through clients and process activity
		for (i = 0; i < *max_fd + 1; ++i)
			if (FD_ISSET(i, cli_fds)) //if the client exists...
				if (i == 0) //...send if it's you...
					send_msg(name, *sock_fd);
				else //...or receive from someone else
					recv_msg(*sock_fd);
	} //end for
} //end run_client

/**
 * Creates a TCP socket and initiates a connection with the server.
 **/
void init_connection(char* ip_addr, fd_set* main_fd, fd_set* cli_fds, int* sock_fd, SA_in* serv_addr)
{
	// Establish socket
	*sock_fd = socket(AF_INET, SOCK_STREAM, 0);

	// Initialize server address structure for connection
	bzero(serv_addr, sizeof(SA_in));
	serv_addr->sin_family = AF_INET;
	serv_addr->sin_port = htons(DEFAULT_SERVER_PORT);
	inet_pton(AF_INET, ip_addr, &(serv_addr->sin_addr));
	
	// Connect to the server
	connect(*sock_fd, (SA*)serv_addr, sizeof(SA));

	// Clear fds before being added to the main fd_set
	FD_ZERO(main_fd);
        FD_SET(0, main_fd);
	FD_ZERO(cli_fds);
	FD_SET(*sock_fd, main_fd);	
} //end init_connection

/**
 * Send a message through the socket to the server (and the other
 * clients).
 **/
void send_msg(char* name, int sock_fd)
{
	char msg_buf[MAXCHARS];

	// Get input from user
	fgets(msg_buf, MAXCHARS, stdin);

	// See if user wants to quit (check the commands)...
	if (strcmp(msg_buf, "/quit\n") == 0 ||
	    strcmp(msg_buf, "/q\n") == 0 ||
	    strcmp(msg_buf, "/disconnect\n") == 0 ||
	    strcmp(msg_buf, "/dc\n") == 0 ||
	    strcmp(msg_buf, "/exit\n") == 0 ||
	    strcmp(msg_buf, "/ex\n") == 0)
	{
		exit(0);
	} //end if
	else // ...or send a message!
	{
		char full_msg[MAXCHARS] = {'0'};
		snprintf(full_msg, sizeof(msg_buf),
			 "%s: %s", name, msg_buf);

		write(sock_fd, full_msg, strlen(full_msg));
	} //end else
} //end send_msg

/**
 * Receive a message from other clients through the socket.
 **/
void recv_msg(int sock_fd)
{
	// Read message from socket
	char msg_buf[MAXCHARS];
	int n = read(sock_fd, msg_buf, MAXCHARS); //num bytes received
	msg_buf[n - 1] = '\0'; //prevent extra bytes from going through

	// Print the message
	printf("%s\n", msg_buf);
} //end recv_msg
