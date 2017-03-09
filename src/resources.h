/***********************************************************************
 * Author: Mgumbo
 *
 * Date Completed: 14 December 2016
 *
 * Description: This header file contains all of the resources needed
 * by the server and client in order to communicate properly. This
 * file is mainly used to prevent clutter in the other files, and
 * serves as the hub for all common items used in both the server and
 * client applications, such as constants and included resources.
 ********************************************************************/

// Include statements
#include <arpa/inet.h> /* htons, htonl */
#include <netinet/in.h> /* various functions and constants */
#include <stdio.h> /* stdin, stdout, printf */
#include <stdlib.h> /* exit */
#include <string.h> /* bzero */
#include <time.h> /* time functions for server logging */

// Constant value definitions
#define DEFAULT_SERVER_PORT 9877
#define LISTENQ 10 //max # of pending connections
#define MAXCHARS 1024 //max # bytes sent between server & client

// Type redefinitions (for more clarity)
typedef struct sockaddr_in SA_in;
typedef struct sockaddr SA;
