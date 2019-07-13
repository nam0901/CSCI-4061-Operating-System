#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <limits.h>

// get_file_size : Returns the file size (in bytes) specified at the
// location pointed to by buf. Returns -1 on error
off_t get_file_size(char* path) {
	if(path == NULL || strlen(path) == 0) {
		return 0;
	}
	struct stat file_stats;
	if(lstat(path, &file_stats) == -1) {
		fprintf(stderr, "%s: %s\n", path, strerror(errno));
		return -1;
	}
	return file_stats.st_size;
}

// free_resources : Closes dir and frees buf to prevent memory leaks
// Assumes buf is dynamically allocated
void free_resources(DIR* dir, char* buf) {
	if(closedir(dir) == -1) {
		fprintf(stderr, "%s\n", strerror(errno));
	}
	free(buf);
}

// directory_tree_walk : Traverses through the directory entries of the directory
// specified at the location pointed to by path. Adds up all the size of non directory
// entries which are descendants of the directory specified at the location
// pointed to by path. Does not include directory file sizes. Returns -1 on error
off_t directory_tree_walk(char* path) {
	size_t path_length = strlen(path);
	if(path == NULL || path_length == 0) {
		return 0;
	}
	
	DIR* directory;
	if((directory = opendir(path)) == NULL) {
		fprintf(stderr, "%s: %s\n", path, strerror(errno));
		return -1;
	}
	
	off_t total_size = 0;
	
	struct dirent* entry;
	while((entry = readdir(directory)) != NULL) {
		// ignore '.' and '..'
		if(strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
			size_t file_name_length = strlen(entry->d_name);
			char* file_path = malloc(path_length + file_name_length + 2);
			if(file_path ==  NULL) {
				fprintf(stderr, "Failed to allocate space\n");
				free_resources(directory, file_path);
				return -1;
			}
			strncpy(file_path, path, path_length + 1);
			strncat(file_path, "/", 1);
			strncat(file_path, entry->d_name, file_name_length);
			// if the entry is a directory, recurse and add all the files' inner size
			if(entry->d_type == DT_DIR) {
				int subdir_size;
				if((subdir_size = directory_tree_walk(file_path)) == -1) {
					// recursive call hit an error at some point
					free_resources(directory, file_path);
					return -1;
				}
				total_size += subdir_size;
			}
			else {
				// just a regular file
				int file_size;
				if((file_size = get_file_size(file_path)) == -1) {
					free_resources(directory, file_path);
					return -1;
				}
				total_size += file_size;
			}
			free(file_path);
		}
	}
	
	// close directory to prevent memory leaks
	if(closedir(directory) == -1) {
		fprintf(stderr, "%s\n", strerror(errno));
	}
	
	return total_size;		
}

// is_directory : Returns 1 if the file specified at the location
// pointed to by path is a directory file, 0 if not a directory file
// Returns -1 on error
int is_directory(char* path) {
	if(path == NULL || strlen(path) == 0) {
		return 0;
	}
	struct stat file_stats;
	if(stat(path, &file_stats) == -1) {
		fprintf(stderr, "%s: %s\n", path, strerror(errno));
		return -1;
	}
	return S_ISDIR(file_stats.st_mode);
}

int main(int argc , char** argv) {
	char* path;
	if(argc == 1) {
		// use current working directory
		if((path = malloc(PATH_MAX * sizeof(char))) == NULL) {
			fprintf(stderr, "Failed to allocate space\n");
			return -1;
		}
		if(getcwd(path, PATH_MAX - 1) == NULL) {
			fprintf(stderr, "%s\n", strerror(errno));
			free(path);
			return -1;
		}
	}
	else {
		path = argv[1];
	}
	
	int is_dir;
	if((is_dir = is_directory(path)) == -1) {
		fprintf(stderr, "%s: %s\n", path, strerror(errno));
		if(argc == 1) {
			free(path);
		}
		return -1;
	}
	
	off_t total_size;
	if(is_dir) {
		if((total_size = directory_tree_walk(path)) == -1) {
			if(argc == 1) {
				free(path);
			}
			return -1;
		}
	}
	else {
		if((total_size = get_file_size(path)) == -1) {
			if(argc == 1) {
				free(path);
			}
			return -1;
		}
	}
	printf("%ld  %s\n", total_size, path);
	
	// if path is dynamically allocated, free the memory
	if(argc == 1) {
		free(path);
	}
	
	return 0;
}
