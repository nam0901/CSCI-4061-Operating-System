#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include "directory.h"

// printcwd : Prints the shell prompt along with the current working directory
int printcwd() {
	// buffer to store the path
	char buf[PATH_MAX];
	// get the current working directory and populate it into buf
	getcwd(buf, sizeof(buf));
	
	if(buf == NULL) {
		// failed to get current working directory
		perror("Error");
		return 1;
	}
	
	const char* shell_template = "[4061-shell]%s $ ";
	printf(shell_template, buf);
	
	// flush stdout to prevent output from not showing due to blocking call from read()
	fflush(stdout);
	
	return 0;
}

// changecwd : Changes the current working directory to path
// Returns 0 upon success, 1 upon failure
int changecwd(char* path) {
	if(path == NULL) {
		// if no path is given, cd to $HOME akin to the builtin cd implementation
		char* home_env = getenv("HOME");
		// if that is not specified, return and do nothing
		if(home_env == NULL) {
			return 0;
		}
		if(chdir(home_env) < 0) {
			perror("cd");
			return 1;
		}
		return 0;
	}
	// path is specified
	if(chdir(path) < 0) {
		perror("cd");
		return 1;
	}
	return 0;
}
