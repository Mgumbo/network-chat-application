# Author: Mgumbo
#
# Date Completed: 14 December 2016
#
# Description: A simple makefile to compile the server and
# client programs. This also executes the server automatically.

#set targets
all: res server client runserv

#compile resources header file
res: resources.h
	cc resources.h

#compile server
server: server.c
	gcc -o server server.c

#compile client
client: client.c
	gcc -o client client.c

#run the server
runserv: server
	./server

.PHONY: all runserv

clean:
	rm *.gch client server
