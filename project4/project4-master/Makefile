CC=gcc
CFLAGS=-std=c99
all : server.c client1.c client2.c
	 $(CC) $(CFLAGS) -o server server.c
	 $(CC) $(CFLAGS) -o client1 client1.c
	 $(CC) $(CFLAGS) -o client2 client2.c
server: server.c
	 $(CC) $(CFLAGS) -o server server.c
clients: client1.c client2.c
	 $(CC) $(CFLAGS) -o client1 client1.c
	 $(CC) $(CFLAGS) -o client2 client2.c
