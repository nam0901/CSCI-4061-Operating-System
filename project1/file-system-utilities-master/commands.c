#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "commands.h"
#include "directory.h"
#include "util.h"

// get_redirect_type: Get the redirect type of the command
Redirect_Type get_redirect_type(char* command) {
	if(command == NULL) {
		return NONE;
	}
	size_t cmd_length =  strlen(command);
	for(int i = 0; i < cmd_length; ++i) {
		if(command[i] == '>') {
			if(i + 1 < cmd_length) {
				if(command[i + 1] == '>') {
					return APPEND;
				}
			}
			return TRUNCATE;
		}
	}
	return NONE;
}

// parse_command : Parses command and populate it into a dynamically allocated 
// array of strings. This array is in the form to be readily executed by execvp()
char** parse_command(char* command) {
	size_t command_length = strlen(command);
	if(command == NULL || command_length == 0) {
		return NULL;
	}
	
	char* space_split[MAX_TOKENS];
	int num_tokens = parse_line(command, space_split, " ");
	// dynamically allocate string array
	char** parsed_command = malloc((num_tokens + 1) * sizeof(char*));
	if(parsed_command == NULL) {
		// failed to allocate space
		return NULL;
	}
	
	for(int i = 0; i < num_tokens; ++i) {
		// do not need to allocate dynamic memory for strings.
		// space_split[i] is pointing to a substring of the original
		// writable input buffer which does not go out of scope 
		// until the shell exits
		parsed_command[i] = space_split[i];
	}
	// null terminate string array. execvp() needs all arrays of strings
	// to be null terminated
	parsed_command[num_tokens] = NULL;
	return parsed_command;
}

// parse_input : The input parser. Parses and populates commands into
// the Command struct array for ease of interpretation / execution
Command* parse_input(char* input, int* status, int* num_commands) {
	// used to store delimited commands strings
	char* command_strs[MAX_TOKENS];
	
	// delimit input by the pipe operator, "|"
	int command_count =  parse_line(input, command_strs, "|");
	
	// pass back the number of commands to the caller
	*num_commands = command_count;
	
	if(command_count == 0) {
		*status = 1;
		return NULL;
	}
	
	// trim the commands
	for(int i = 0; i < command_count; ++i) {
		// since command_strs[i] points to a substring of the writable input
		// buffer, it would be writable. This would not lead to a segfault
		// once passed into trim_trailing_whitespaces.
		command_strs[i] = trim_trailing_whitespaces(command_strs[i]);
	}
	
	Command* commands = malloc(command_count * sizeof(Command));
	if(commands == NULL) {
		// failed to allocate space
		*status = 1;
		return NULL;
	}
	
	// delimit the individual commands via the space character and populate the Command array
	for(int i = 0; i < command_count; ++i) {
		// set flag to 0 (flag would be set to 1 if indeed memory is allocated for commands[i].command)
		commands[i].command_allocated = 0;
		
		// set the redirect_type field
		commands[i].redirect_type = get_redirect_type(command_strs[i]);
		
		if(commands[i].redirect_type != NONE) {
			char* operator_split[MAX_TOKENS];
			
			// split on the redirect operator
			int num_tokens = parse_line(command_strs[i], operator_split, ">");
			if(num_tokens < 2) {
				// command error, either no command is supplied or no output destination is supplied
				*status = 1;
				return commands;
			}
			
			// parse and format command. Command should be located in operator_split[0]
			commands[i].command = parse_command(operator_split[0]);
			if(commands[i].command == NULL) {
				// failed to allocate space
				*status = 1;
				return commands;
			}
			
			// commands[i].redirect_path does not need to have its memory dynamically allocated
			// operator_split[1] is pointing to a substring of the original input buffer, which does
			// not go out of scope until the shell exits. Since that buffer is writable, passing this
			// substring to trim_trailing_whitespaces would not result in a segfault either
			commands[i].redirect_path = trim_trailing_whitespaces(operator_split[1]);
		} else {
			// no redirection
			commands[i].command = parse_command(command_strs[i]);
			if(commands[i].command == NULL) {
				// failed to allocate space
				*status = 1;
				return commands;
			}
		}
		// mark commands[i].command as 'Allocated'
		commands[i].command_allocated = 1;
	}
	*status = 0;
	return commands;
}

// exec_commands : Build the pipeline and execute commands
// Returns 0 upon success, 1 upon failure
int exec_commands(Command* commands, int num_commands) {
	if(num_commands < 1) {
		return 0;
	}
	
	// since the commands are delimited by pipes, the number of pipes
	// needed to build the pipeline would be equal to (num_commands - 1)
	int num_pipes = num_commands - 1;
	
	// build the pipe ends
	int pipe_ends[num_pipes][2];
	
	// connect the pipe ends together and build the pipeline
	for(int i = 0; i < num_pipes; ++i) {
		pipe(pipe_ends[i]);
	}
		
	// spawn child processes to execute the commands
	for(int i = 0; i < num_commands; ++i) {
		enum command_type command_name = get_command_type(commands[i].command[0]);
		
		// first command length (excluding args)
		size_t command_length = strlen(commands[i].command[0]);
		// needed in case of du and grep where the string has to be prepended with "./"
		char edited_command[3 + command_length];
		
		switch(command_name) {
			case CD:
				changecwd(commands[i].command[1]);
				continue;
				
			case DU:
			case GREP:
				// call our du and grep programs instead of the builtin ones
				// this can be done by prepending "./" to the command
				
				// append "./" to the newly allocated buffer
				// an assumption is being made that the current working directory
				// is the working directory in which the shell executable lives.
				strncpy(edited_command, "./", 3);
				// append the command name
				strncat(edited_command, commands[i].command[0], command_length);
				// point commands[i].command[0] to the newly allocated string
				// Since the old buffer which commands[i].command[0] pointed to is
				// not dynamically allocated, memory leaks would not occur here
				// Since that buffer memory was allocated in shell, it would be
				// deallocated when shell exits
				commands[i].command[0] = edited_command;
				break;
			
			case EXIT:
				// free memory before exiting
				free_memory(commands, num_commands);
				printf("Exiting shell\n");
				exit(0);
		}
		
		// spawn a child process to execute command
		pid_t child_pid = fork();
		if(child_pid == 0) {
			// child code
			int prev_pipe_number = i - 1;
			
			// if this is not the first command...
			if(i != 0) {
				// close write end of previous pipe
				close(pipe_ends[prev_pipe_number][1]);
				// redirect stdin to read end of pipe
				if(dup2(pipe_ends[prev_pipe_number][0], STDIN_FILENO) < 0) {
					// failed to redirect
					exit(1);
				}
			} else {
				// if this is the first command, no redirect needed
			}
			
			if(commands[i].redirect_type != NONE) {
				int fd, flags;
				if(commands[i].redirect_type == TRUNCATE) {
					flags = O_WRONLY | O_CREAT | O_TRUNC;
				} else { // APPEND
					flags = O_WRONLY | O_CREAT | O_APPEND;
				}
				
				// open the file at the specified redirect path
				fd = open(commands[i].redirect_path, flags);
				if(fd < 0) {
					// failed to open file
					exit(1);
				}
				
				// read, write, execute permissions
				if(chmod(commands[i].redirect_path, 0777) < 0) {
					perror("Error");
					exit(1);
				}
				
				// redirect stdout to fd
				if(dup2(fd, STDOUT_FILENO) < 0) {
					// failed to redirect
					exit(1);
				}	
			}
			
			// if this is not the last command...
			if(i != num_commands - 1) {
				int next_pipe_number = i;
				// close read end of next pipe
				close(pipe_ends[next_pipe_number][0]);
				// redirect stdout to write end of next pipe
				if(dup2(pipe_ends[next_pipe_number][1], STDOUT_FILENO) < 0) {
					// failed to redirect
					exit(1);
				}
			} else {
				// if this is the last command, do nothing.
				// if this command has redirection, it would have
				// been handled earlier. If not, the output would
				// just print to stdout
			}
			
			// execute command
			execvp(commands[i].command[0], commands[i].command);
			// if control reaches here, child failed to execute process
			exit(1);
		} else if(child_pid == -1) {
			// failed to fork
			// wait for any previously forked child processes to terminate before exiting
			while(wait(NULL) > 0);
			return 1;
		}
	}	
	// wait for all child processes to terminate
	int status = 0, retval = 0;
	for(int i = 0; i < num_commands; ++i) {
		wait(&status);
		if (!(WIFEXITED(status) && !WEXITSTATUS(status))) {
			retval = 1;
		}
	}
	return retval;
}

// free_memory : Free commands, including its internal properties
// which point to dynamically allocated memory
void free_memory(Command* commands, int num_commands) {
	if(commands == NULL) {
		return;
	}
	
	for(int i = 0; i < num_commands; ++i) {
		// do not need to free individual char* elements
		// as they are not dynamically allocated
		
		// free array of char* (char**)
		if(commands[i].command_allocated) {
			free(commands[i].command);
		}
	}
	// free outermost layer
	free(commands);
}
