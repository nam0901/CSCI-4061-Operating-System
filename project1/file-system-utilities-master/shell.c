#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "commands.h"
#include "directory.h"
#include "util.h"

int main () {
  char buf[MAX_CMD_LEN];

  while(1) {
	  // print the shell template along with the current working directory
	  printcwd();
	  
	  ssize_t bytes_read = read(STDIN_FILENO, buf, sizeof(buf));
	  if(bytes_read < 0) {
		  perror("Error");
		  continue;
	  }
	  
	  // null terminate and trim the input
	  buf[bytes_read] = '\0';
	  char* input = trim_trailing_whitespaces(buf);
	  
	  if(strlen(input) > 0) {
		  int status, num_commands;
		  
		  Command* commands = parse_input(buf, &status, &num_commands);
		  if(status == 1 || commands == NULL) {
			  fprintf(stderr, "Command Error\n");
		  } else {
				if(exec_commands(commands, num_commands) !=  0) {
					fprintf(stderr, "Command Error\n");
				}
		  }
		  // commands have been executed, free the memory for upcoming commands
		  free_memory(commands, num_commands);
	  }
  }
  return 0;
}
