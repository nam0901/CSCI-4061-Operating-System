#ifndef DIRECTORY_H
#define DIRECTORY_H

/*
 * printcwd : Prints the shell prompt along with the current working directory
 * Returns 0 upon success, 1 upon failure
 */
int printcwd();

/*
 * changecwd : Changes the current working directory to path
 * Returns 0 upon success, 1 upon failure
 */
int changecwd(char* path);

#endif
