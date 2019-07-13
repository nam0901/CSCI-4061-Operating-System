#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "util.h"

#define BUFSIZE 10000

// get_file_size : Returns the file size (in bytes) specified at the
// location pointed to by buf. Returns -1 on error
off_t get_file_size(char* path) {
	if(path == NULL || strlen(path) == 0) {
		return 0;
	}
	struct stat file_stats;
	if(stat(path, &file_stats) == -1) {
		fprintf(stderr, "%s: %s\n", path, strerror(errno));
		return -1;
	}
	return file_stats.st_size;
}

// search : Searches for the presence of needle in the haystack. 
// If needle is present, it prints out the corresponding line in the haystack.
void search(char* needle, char** haystack, int num_strings) {
	if(needle == NULL || haystack == NULL) {
		return;
	}
	
	// print out all lines where needle exists
	for(int i = 0; i < num_strings; ++i) {
		// if keyword is found in the line
		if(strstr(haystack[i], needle) != NULL) {
			// print that line to stdout
			printf("%s\n", haystack[i]);
		}
	}
}

int main(int argc , char *argv[])
{
	if (argc < 2) {
		fprintf(stderr, "Either a keyword and/or a filename (optional) must be passed\n");
		return 1;
	}
	
	// the keyword to seach for
	char* keyword = argv[1];
	
	// used to track the number of bytes read from stream
	ssize_t bytes_read;
	
	// number of delimited lines read from the stream
	int num_lines;
	
	// array of strings used to store delimited lines in a stream
	char* lines[MAX_TOKENS]; // assumes there cannot be more than 999 lines (1 element for null termination)
	
	// read from STDIN (this read occurs in an infinite loop akin to the builtin grep implementation)
	if(argc == 2) {
		// buffer to read input from stdin
		char buf[BUFSIZE];
		
		bytes_read = read(STDIN_FILENO, buf, sizeof(buf) - 1); // reserve 1 byte for null termination
		
		if(bytes_read < 0) {
			fprintf(stderr, "grep: %s: %s\n", argv[2], strerror(errno));
			return 1;
		}
		
		// null terminate buf
		buf[bytes_read] = '\0';
		
		// delimit the input by the '\n' character
		num_lines = parse_line(buf, lines, "\n");
		
		// search and print lines containing keywords
		search(keyword, lines, num_lines);
		
		return 0;
	}
	
	// read from files
	for(int i = 2; i < argc; ++i) {	
		// open file in readonly mode
		int fd = open(argv[i], O_RDONLY);
		if(fd == -1) {
			fprintf(stderr, "grep: %s: %s\n", argv[2], strerror(errno));
			continue;
		}
		
		// get the file size
		off_t file_size = get_file_size(argv[i]);
		if(file_size == -1) {
			// error getting file size
			close(fd);
			continue;
		}
		
		char* buf = malloc(file_size + 1); // one byte for null termination
		if(buf == NULL) {
			// failed to allocate space
			fprintf(stderr, "grep: %s: %s\n", argv[i], strerror(errno));
			close(fd);
			continue;
		}
			
	
		// read into buf
		bytes_read = read(fd, buf, file_size); // reserve 1 byte for null termination
		
		// null terminate buf
		buf[bytes_read] = '\0';
		
		// close the file
		close(fd);
		
		if(bytes_read < 0) {
			fprintf(stderr, "grep: %s: %s\n", argv[i], strerror(errno));
			free(buf);
			continue;
		}
		
		// delimit the input by the '\n' character
		num_lines = parse_line(buf, lines, "\n");
		
		// search and print lines containing keywords
		search(keyword, lines, num_lines);
		
		free(buf);
	}
	
	return 0;
}
