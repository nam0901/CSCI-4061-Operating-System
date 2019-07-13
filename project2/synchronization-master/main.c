#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "linked_list.h"

#define NUM_ARGS 3
#define OUT_FILE "copy.txt"

// global variables used to control state
static int next_seq_num = 0;
static int next_line_num = 0;
static int num_lines = 1;
static pthread_mutex_t *seq_num_lock = NULL;
static pthread_mutex_t *line_num_lock = NULL;
static int offsets[MAX_LINES]; // array used to store offsets

void preprocess(char* filename) {
	// open the connection to the file
	FILE* fp;
	if((fp = fopen(filename, "r")) == NULL) {
		perror("fopen");
		exit(1);
	}
	
	// read from fp and get the number of lines in the file and store offsets
	offsets[0] = 0;
	char c;
	int num_bytes = 0;
	do {
		c = fgetc(fp);
		if(c == '\n') offsets[num_lines++] = num_bytes + 1;
		num_bytes++;
	} while(c != EOF);
	
	num_lines--; // negate last line as it's EOF
	
	fclose(fp);
	return;
}

int get_line_num() {
	// lock critical section
	if(pthread_mutex_lock(line_num_lock) != 0) {
		perror("pthread_mutex_lock");
		free_resources(head);
		exit(3);
	}
	
	int line_num = next_line_num;
	next_line_num++;
	
	// unlock critical section
	if(pthread_mutex_unlock(line_num_lock) != 0) {
		perror("pthread_mutex_unlock");
		free_resources(head);
		exit(3);
	}
	return line_num;
}

void *run(void* filename) {
	while(next_line_num < num_lines) {
		// get a line to read
		int line_num = get_line_num();

		// nothing more to read
		if(line_num >= num_lines) return NULL;

		// read the line
		char* line = read_line((char*)filename, line_num);

		// create node
		node* line_node = create_node(line_num, line);

		// insert the node into the linked list
		insert(head, line_node);
	}
	return NULL;
}

char* read_line(char* filename, int line_num) {
	// open the connection the file
	FILE* fp;
	if((fp = fopen(filename, "r")) == NULL) {
		perror("fopen");
		free_resources(head);
		exit(3);
	}
	
	char *line;
	if((line = malloc(sizeof(LINE_SIZE))) == NULL) {
		free_resources(head);
		exit(3);
	}
	
	// set pointer to the start of the line
	if(lseek(fileno(fp), offsets[line_num], SEEK_SET) == (off_t) -1) {
		perror("lseek");
		free_resources(head);
		exit(3);
	}
	
	// read the line into line
	if(fgets(line, LINE_SIZE, fp) ==  NULL) {
		free_resources(head);
		exit(3);
	}
	
	// calling fclose on fp causes a memory corruption
	// no idea why. Not closing fp may lead to mem leaks, have to resolve
	// fclose(fp);
	
	return line;
}

node* create_node(int line_num, char *line) {
	node* line_node;
	if((line_node = malloc(sizeof(node))) == NULL) {
		free_resources(head);
		exit(3);
	}
	
	line_node->line_no = line_num;
	line_node->content = line;
	line_node->next = NULL;
	
	return line_node;
}

void insert(node *head, node *new) {
	if(pthread_mutex_lock(seq_num_lock) != 0) {
		perror("pthread_mutex_lock");
		free_resources(head);
		exit(3);
	}
	
	new->seq_no = next_seq_num;
	
	node* prev = NULL, *curr = head;
	while(curr && curr->line_no < new->line_no) {
		prev = curr;
		curr = curr->next;
	}

	prev->next = new;
	new->next = curr;
	
	next_seq_num++;
	
	if(pthread_mutex_unlock(seq_num_lock) != 0) {
		perror("pthread_mutex_unlock");
		free_resources(head);
		exit(3);
	}
	return;
}

void traversal(node *head) {
	FILE* fp;
	if((fp = fopen(OUT_FILE, "w")) == NULL) {
		perror("fopen");
		exit(1);
	}
	
	node* curr = head->next;
	while(curr) {
		fprintf(fp, "%3d, %3d, %s", curr->seq_no, curr->line_no, curr->content);
		curr = curr->next;
	}
	
	fclose(fp);
	return;
}

void free_resources(node* head) {
	// free mutex locks
	free(seq_num_lock);
	free(line_num_lock);

	node *prev = NULL, *curr = head;
	while(curr) {
		// same as the fp issue above, can't seem to free() certain lines
		// got a free() : invalid cd next size (fast) error here when done on
		// certain sets of inputs
		// if(prev && prev != head) free(prev->content);
		
		// free node
		free(prev);
		prev = curr;
		curr = curr->next;
	}
	
	// free last node
	free(prev->content);
	free(prev);
	
	return;
}
			
int main(int argc, char** argv) {
	if(argc < NUM_ARGS) {
		printf("Wrong number of arguments. Expected %d, given %d.\n", NUM_ARGS - 1, argc - 1);
		return 0;
	}
	
	char* filename = argv[1];
	int num_threads = atoi(argv[2]);
	
	if(num_threads < 1 || num_threads > 16) {
		printf("Invalid number of threads. Expected value between %d and %d, given %d.\n", MIN_THREADS, MAX_THREADS, num_threads);
		return 0;
	}
	
	// define head to determine whether the linked list has been allocated or not in free_resources()
	head = NULL;
	
	// read the file once to get the proper offsets
	preprocess(filename);
	
	// the head of the linked list is just an auxiliary node with no content.
	// the first line of the file would be pointed to by head->next.
	if((head = malloc(sizeof(node))) == NULL) exit(1);
	head->line_no = -1;
	head->next = NULL;
	
	// allocate memory for threads
	pthread_t pool[num_threads];
	
	// initialize mutex locks
	if((seq_num_lock = malloc(sizeof(pthread_mutex_t))) == NULL) exit(1);
	if(pthread_mutex_init(seq_num_lock, NULL) != 0) {
		perror("pthread_mutex_init");
		exit(1);
	}
	
	if((line_num_lock = malloc(sizeof(pthread_mutex_t))) == NULL) exit(1);
	if(pthread_mutex_init(line_num_lock, NULL) != 0) {
		perror("pthread_mutex_init");
		exit(1);
	}
	
	// spawn threads
	for(int i = 0; i < num_threads; i++) {
		if(pthread_create(&pool[i], NULL, run, (void*)filename) != 0) {
			perror("pthread_create");
			free_resources(head);
			exit(3);
		}
	}
	
	// join threads, code reaches here only after all lines have been read
	for(int i = 0; i < num_threads; i++) {
		if(pthread_join(pool[i], NULL) != 0) {
			perror("pthread_join");
			free_resources(head);
			exit(3);
		}
	}
	
	// output linked list content
	traversal(head);
	
	// free dynamically allocated resources before return
	free_resources(head);
	
	return 0;
}
