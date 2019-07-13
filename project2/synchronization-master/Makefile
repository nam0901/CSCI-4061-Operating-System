CC=gcc
DBFLAGS=-fsanitize=address

all: main.c linked_list.h
	$(CC) main.c -pthread -o tlist

debug: main.c linked_list.h
	$(CC) main.c -pthread -o tlist $(DBFLAGS)
clean:
	rm -rf tlist
