#ifndef COMMANDS_H
#define COMMANDS_H

// Redirect_Type represents the type of file redirection that is need to be made
typedef enum redirect_t{TRUNCATE, APPEND, NONE} Redirect_Type;

// The Command struct represents a shell command / executable.
typedef struct {
	char** command; // command delimited by the space character. In appropriate form to be called by execvp()
	int command_allocated; // boolean which is set to 1 if command is allocated dynamic memory, 0 if not. This is to prevent calling free() on unallocated memory
	char* redirect_path; // path to dump command output. If the destination is the default stdout, this field will be NULL
	enum redirect_t redirect_type;
} Command;

/* 
 *  parse_input : Parses the input string and populates it into a Command struct array.
 *  On error it sets the dereference of status to 1, else 0.
 *  num_commands is used to notify the caller of the number of commands parsed. 
 */
Command* parse_input(char* input, int* status, int* num_commands);

/*
 * exec_commands: Builds the pipeline(if any) and spawns child processes to execute
 * the commands. Returns 0 upon success, 1 upon failure
 */
int exec_commands(Command* commands, int num_commands);

/* 
 *  free_memory : Free commands, including its internal properties
 *  which point to dynamically allocated memory. 
 */
void free_memory(Command* commands, int num_commands);

#endif
