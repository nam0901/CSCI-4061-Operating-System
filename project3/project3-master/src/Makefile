CC=gcc
CFLAGS=-std=c99
DBFLAGS=-g

all: main.c vmemory.o vmemory.h
	$(CC) $(CFLAGS) -o vmanager ../bin/page_table.o vmemory.o main.c

clean: 
	rm -rf vmemory.o vmanager page_table.o
	rm -rf ../bin/tlb_out.txt
