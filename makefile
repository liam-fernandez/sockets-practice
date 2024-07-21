all: prog

prog: client server

client: client.o
	gcc client.o -o client

server: server.o
	gcc server.o -o server

client.o: client.c constants.h
	gcc -g -c client.c 

server.o: server.c constants.h
	gcc -g -c server.c 

clean:
	rm *.o client server 07-20-server
