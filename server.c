/***********************************************************************
 * Author: Mgumbo
 *
 * Date Completed: 14 December 2016
 *
 * Description: A program that functions as a server for a chat program.
 * Establishes connections with incoming clients, and then transmits
 * messages sent by any given client at any given time to all other
 * clients connected to the server. Any connections to, messages sent
 * through, or disconnections from the server are recorded in a basic
 * log file. Handling of multiple clients is done with the select()
 * function.
 ***********************************************************************/
#include "resources.h"

// Function prototypes
void run_server(int* serv_fd, fd_set* cli_fds, fd_set* main_fd, int* max_fd, SA_in* cli_addr);
void init_server(fd_set* main_fd, fd_set* cli_fds, int* serv_fd, SA_in* serv_addr);
void accept_client(int id, fd_set* main_fd, int* max_fd, int serv_fd, SA_in* cli_addr);
void process_client(int id, fd_set* main_fd, int serv_fd, int max_fd);
void log_activity(char* msg);

/**
 * Runs the program.
 **/
int main(int argc, char** argv)
{
	// Local variables
	fd_set main_fd, cli_fds; //fd sets for main server and clients
	int serv_fd = 0, max_fd; //fd for server and max fd for select
	SA_in serv_addr, cli_addr; //address structs for server and clients
	
	// Do all pre-connection establishment for server properties
	init_server(&main_fd, &cli_fds, &serv_fd, &serv_addr);

	// Until a higher fd connects, server is highest level fd
	max_fd = serv_fd;

	// Begin server processing
	run_server(&serv_fd, &cli_fds, &main_fd, &max_fd, &cli_addr);

	return 0;
} //end main

/**
 * Begin running an infinite loop to process client connections as well
 * as client input. Runs through a loop when activity occurs, checks which
 * kind of activity was detected, and performs appropriate actions based
 * on the activity.
 **/
void run_server(int* serv_fd, fd_set* cli_fds, fd_set* main_fd, int* max_fd, SA_in* cli_addr)
{
	// Infinite loop to process clients
	int i; //loop control
	for (;;)
	{
		*cli_fds = *main_fd;
		
		// When client activity occurs, store that client's fd
		// in cli_fds for processing
		select(*max_fd+1, cli_fds, NULL, NULL, NULL);
		
		// Cycle through clients and see which one had activity
		for (i = 0; i < *max_fd + 1; i++)
		{
			// If the current client is the active one
			if (FD_ISSET(i, cli_fds))
			{
				// Check if it is a new client connecting...
				if (i == *serv_fd)
					accept_client(i, main_fd, max_fd, *serv_fd, cli_addr);
				else //...or an existing client sending a msg
					process_client(i, main_fd, *serv_fd, *max_fd);
			} //end if
		} //end for
	} //end infinite for	
} //end process_clients

/**
 * Ensure the server is properly set up before connections can be made.
 * Creates the socket, ensures the socket can handle multiple connections,
 * binds the socket and server address, and allows the server to listen.
 **/
void init_server(fd_set* main_fd, fd_set* cli_fds, int* serv_fd, SA_in* serv_addr)
{
	// Local variables
	int reuse = 1; //TCP address reusability option

	// Clear fds and prepare to accept a connection
	FD_ZERO(main_fd);
	FD_ZERO(cli_fds);
		
	// Initialize server address structure attributes
	bzero(serv_addr, sizeof(SA_in)); //also sets unused bytes to 0
	serv_addr->sin_family = AF_INET;
	serv_addr->sin_port = htons(DEFAULT_SERVER_PORT);
	serv_addr->sin_addr.s_addr = htonl(INADDR_ANY);

	// Attempt to create TCP socket
	*serv_fd = socket(AF_INET, SOCK_STREAM, 0);		
		
	// Allow address reuse so multiple clients can connect
	setsockopt(*serv_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

	// Bind socket and server address
	bind(*serv_fd, (SA *)serv_addr, sizeof(SA));

	// Listen for up to LISTENQ clients
	listen(*serv_fd, LISTENQ);

	// Store server fd in main fd set
	FD_SET(*serv_fd, main_fd);

	// Print out message for anyone viewing server; this not logged	
	printf("Terminate server at any time with CTRL + c.\n");
	printf("Current port: %d\n", DEFAULT_SERVER_PORT);
	printf("Waiting for clients to connect...\n");
} //end init_server		

/**
 * If an incoming connection is requested, get the client's information
 * and add them to the main fd_set, allowing them to communicate through
 * the server.
 **/
void accept_client(int id, fd_set* main_fd, int* max_fd, int serv_fd, SA_in* cli_addr)
{
	// Accept a new connection
	socklen_t addrlen = sizeof(SA_in);
	int new_fd = accept(serv_fd, (SA*)cli_addr, &addrlen);
	
	// Make sure client connected properly
	if(new_fd < 0)
	{
		printf("Error accepting client\n");
		exit(1);
	} //end if
	else
	{
		// Add new client to fd set
		FD_SET(new_fd, main_fd);
		
		// If new client fd is highest, record it for select
		if(new_fd > *max_fd)
			*max_fd = new_fd;

		// Log client connection to server
		char activity[75];
		snprintf(activity, sizeof(activity),
			"Client connected from %s on port %d\n",
			inet_ntoa(cli_addr->sin_addr), ntohs(cli_addr->sin_port));
		log_activity(activity);
	} //end else
} //end accept_client

/**
 * If an existing client performs any action, check the action. If
 * they disconnected, remove them from the fd_set; otherwise, if they
 * sent a message, read the message and relay it to all other clients.
 **/			
void process_client(int id, fd_set* main_fd, int serv_fd, int max_fd)
{
	// Local variables
	int n; //number of bytes received
	int j; //used for loops
	char msg_buf[MAXCHARS];
	
	// If no message received, the client disconnected
	if ((n = read(id, msg_buf, MAXCHARS)) < 1)
	{
		// Log client disconnection from server
		char activity[24];
		snprintf(activity, sizeof(activity),
			 "Socket %-2d disconnected\n", id);
		log_activity(activity);

		// Close the connection with ex-client
		close(id);
		FD_CLR(id, main_fd);
	} //end if
	else // Otherwise, a message was received
	{ 
		int logged = 0; //track if logged already; default to false

		// Send the message to all clients except the one who sent it
		for(j = 0; j < max_fd + 1; j++)
		{
			// Output to other clients
			if (FD_ISSET(j, main_fd)) //ensure client exists
				if (j != serv_fd && j != id) //ignore server and sender
					write(j, msg_buf, n);

			// Log messages sent by clients only once; placing
			// this code here ensures that even if only 1 client
			// is connected, sent messages are still recorded
			if (logged == 0)
			{
				// Log messages sent by clients
				msg_buf[n] = '\0'; //for logging
				log_activity(msg_buf);
				logged = 1;
			} //end if
		} //end for
	} //end else	
} //end process_client

/**
 * Logs a given message to the server log, as well as the
 * time the message occurred. Also prints to the server
 * console. If the log file doesn't already exist, it will
 * be created.
 **/
void log_activity(char* msg)
{	
	// Local variables
	FILE* log_file = fopen("serverlog.txt", "a"); //serverlog file
	time_t raw_time; //raw time data
	struct tm* time_info; //structured time data

	// Format the time and store as a char array
	time(&raw_time);
	time_info = localtime(&raw_time);
	char* time_str = asctime(time_info); //get formatted time
	time_str[strlen(time_str)-1] = '\0';

	// Print to the server console and log into file
	printf("[%s]%s", time_str, msg);
	fprintf(log_file, "[%s]%s", time_str, msg);

	// Close the connection	
	fclose(log_file);
} //end log
