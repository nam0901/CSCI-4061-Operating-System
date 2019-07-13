#ifndef __LINKED_LIST_H
#define __LINKED_LIST_H

#define LINE_SIZE 256
#define MAX_LINES 200
#define MIN_THREADS 1
#define MAX_THREADS 16

typedef struct linked_node {
	int seq_no;
	int line_no;
	char *content;
	struct linked_node *next;	
} node;

//Read a particular line from file
//param 1: file name, param 2: line_no
char* read_line(char *, int); 

//Traverse the linked list with the head as the argument
void traversal(node *);

//Insert a node created using create_node into the linked list
//param 1: head, param 2: node
void insert(node *, node *);

//Create a new node to be inserted into the linked list
//param 1: line_no; param 2: line content
node* create_node(int, char *);

// Free dynamically allocated variables in linked list
// param 1: head of linked list
void free_resources(node *);

//The head ptr of the linked list
node *head;
#endif
