# File System Utilities

## Contributions
### Jan-Wei Lim (limxx517)
- Built the input parser
- Implemented `grep`
- Implemented pipes, pipelines.

### Nam Ho (hoxxx395)
- Implemented `cd`
- Implemented `du`
- Drafted documentation and ran test cases.

### Jacob Bandyk (bandy036)
- Implemented `exit`
- Implemented output redirection.
- Wrote documentation and ran test cases.

## Overview
This project focuses on the implementation of the Unix Shell and how they actually operate.
Shells fork child processes in order to execute commands/programs and handle errors that are
returned to them. This project implements a custom Unix shell which emulates how the builtin 
Unix Terminal works. This shell is able to pipe commands and handle output redirection. 
Alongside the shell, we have also implemented separate programs for `cd`, `du`, `grep` and `exit`.

## Running the project

### Clone the project
Clone the project to your local via HTTPS or SSH :
```
git clone https://github.umn.edu/limxx517/file-system-utilities.git
```

### Compile the project
To compile all source files in this project, just execute the following command :
```
make all
```
This would compile our custom shell, `du` and `grep` implementations all at once!
Besides, you could compile the individual programs separately by calling the following commands :

- `make shell` to compile the shell
- `make grep` to compile `grep`
- `make du` to compile `du` 

### Running the shell
To run the shell, `cd` into the shell's working directory and execute the following command :
```
./shell
```
This would invoke our shell and you should be within our shell environment.

To exit the shell, invoke the following command :
```
exit
```

### Running `cd`
In order for the `cd` command to take effect, the `cd` command **must** be executed within our shell environment.
```
cd <directory-path>
```
This would change the current working directory to `directory-path`. If no `directory-path` is specified, the directory would
be changed to the directory specified by the `$HOME` environment variable.

### Running `du`
If you are within our shell environment, just enter the command as you would in the original Terminal.

(**IMPORTANT**: You **must** be in the working directory in which the `du` executable lies in order 
for this to work as the absolute path to `du` is not prepended to `$PATH`)
```
du <path>
```
If you are not within our shell environment, just prepend `./` to `du` (this is assuming `du` is executed within the
working directory where the `du` executable lies.)
```
./du <path>
```
This would output a single value, representing the total number of bytes taken up by all the descendant files of the directory specified by `path`. If `path` is not specified, the current working directory is used as the path. More details about `du` below.

### Running `grep`
If you are within our shell environment, just enter the command as you would in the original Terminal.

(**IMPORTANT**: You **must** be in the working directory in which the `grep` executable lies in order 
for this to work as the absolute path to `grep` is not prepended to `$PATH`)
```
grep <keyword> <file1> <file2> ...
```
If you are not within our shell environment, just prepend `./` to `grep` (this is assuming `grep` is executed within the
working directory where the `grep` executable lies.)
```
./grep <keyword> <file1> <file2> ...
```
This would print the lines in file1, file2, ... which contains `keyword`. If no file is specified, `grep` takes input from
`stdin` (Terminal). More details about `grep` below.

### Piping commands
This shell supports multiple pipes! Just separate commands by the pipe operator, `|`.
```
command1 | command2 | command3 ...
```

### Redirecting output
This shell supports output redirection. Instead of printing output to `stdout` (Terminal), output can be redirected to files
specified by the user. There are two modes in which output redirection can be performed : 

- Truncate
- Append

#### Truncate
```
command1 > file_name.txt
```
This command redirects `command1`'s output to `file_name.txt`. If `file_name.txt` does not exist, it creates the file and writes the contents. If `file_name.txt` exists, it truncates the contents of the file before writing the output.

#### Append
```
command1 >> file_name.txt
```
This command redirects `command1`'s output to `file_name.txt`. If `file_name.txt` does not exist, it creates the file and writes the contents.
If `file_name.txt` exists, it appends `command1`'s output to the contents of `file_name.txt`.

### Combining pipes and output redirection
Commands can chain pipes and redirect output to a specified file.
```
command1 | command2 | command3 ... > file_name.txt
```
This command would pipe commands `command1`, `command2`, `command3` and so on and the final command's output would be redirected to `file_name.txt` (Truncate mode)

```
command1 | command2 | command3 ... >> file_name.txt
```
This command would pipe commands `command1`, `command2`, `command3` and so on and its output would be appended to `file_name.txt`

## Additional Details
 
### Shell
This custom Unix shell supports the following operations :

- Pipes (multiple pipes also do work!)
- Output redirection (Both Truncate and Append via the `>` and `>>` operator)
- Combining pipes and output redirection.

This shell does **NOT** support the following operations :

- Sequential operations via the `&&` operator
- Parallel operations via the `&` operator
- Input redirection via the `<` operator
- Here document via the `<<` operator
- Other operators which do not include pipes or output redirection.

The shell would execute builtin commands/executables akin to the original Terminal. However, if the command entered
is any of the four implemented in this project (`cd`, `du`, `grep`, `exit`), the shell would call our implementations instead of the builtin ones.

### `du`
- `du` outputs a single value representing the total size in bytes of the descendant files of the file/directory requested. 
If no path is specified, it uses the current working directory.
- If a path pointing to a regular file is given, the size of the file is returned. If the path provided is a directory, `du` sums up 
the size of all the files in that subdirectory. If that subdirectory contains further subdirectories, those subdirectories are 
opened and the file sizes are added to the total. This operation recursively calls itself till contents of all subdirectories are summed up.
- The subdirectories' sizes are not added to the grand total.
- If symbolic links are present, `du` sums up the size of those symbolic links instead of the size of the files that those symbolic links point to.

### `grep`
- `grep` outputs the lines contained in the given files in which the specified keyword is present. If no files are given, `grep` takes input from `stdin` (Terminal)
- This version of `grep` does **not** support regular expressions.
- Multiple files can be searched at once. Just append the names of the files to be searched to the command.


## Test Cases

### Invoking the Shell
```
limxx517@csel-vole-15:/home/limxx517/Documents/file-system-utilities $ ./shell
[4061-shell]/home/limxx517/Documents/file-system-utilities $
```

### Testing `cd`

#### Providing a relative path
```
[4061-shell]/home/limxx517/Documents/file-system-utilities $ cd ..
[4061-shell]/home/limxx517/Documents $
```

#### Providing an absolute path
```
[4061-shell]/home/limxx517/Documents/file-system-utilities $ cd /home/limxx517/Desktop
[4061-shell]/home/limxx517/Desktop $
```

#### Providing no path (changes to the directory specified by the `$HOME` environment variable)
```
[4061-shell]/home/limxx517/Documents/file-system-utilities $ cd
[4061-shell]/home/limxx517 $
```

### Testing `du`

#### Providing no path (Uses the current working directory as the path)
Output mirrors that of the executable provided by the TA (`du_sol`)
```
[4061-shell]/home/limxx517/Documents/file-system-utilities $ du
234521  /home/limxx517/Documents/file-system-utilities
[4061-shell]/home/limxx517/Documents/file-system-utilities $ ./du_sol
234521	/home/limxx517/Documents/file-system-utilities
[4061-shell]/home/limxx517/Documents/file-system-utilities $
```

#### Providing a relative path
Output mirrors that of the executable provided by the TA (`du_sol`)
```
[4061-shell]/home/limxx517/Documents/file-system-utilities $ du ../../Desktop
1248220  ../../Desktop
[4061-shell]/home/limxx517/Documents/file-system-utilities $ ./du_sol ../../Desktop
1248220	../../Desktop
```

#### Providing an absolute path
Output mirrors that of the executable provided by the TA (`du_sol`)
```
[4061-shell]/home/limxx517/Documents/file-system-utilities $ du /home/limxx517
624088548  /home/limxx517
[4061-shell]/home/limxx517/Documents/file-system-utilities $ ./du_sol /home/limxx517
624088548	/home/limxx517
```

### Testing `grep`

#### Providing no file (Input is taken from `stdin` (Terminal))
```
[4061-shell]/home/limxx517/Documents/file-system-utilities $ grep test
test123
test123
```

#### Providing a file
```
[4061-shell]/home/limxx517/Documents/file-system-utilities $ ls -l > output.txt
[4061-shell]/home/limxx517/Documents/file-system-utilities $ grep commands output.txt
-rw------- 1 limxx517 CSEL-student  9538 Mar 11 19:18 commands.c
-rw------- 1 limxx517 CSEL-student  1350 Mar 11 19:18 commands.h
```

### Testing pipes

#### One pipe
```
[4061-shell]/home/limxx517/Documents/file-system-utilities $ ls -l | grep commands
-rw------- 1 limxx517 CSEL-student  9538 Mar 11 19:18 commands.c
-rw------- 1 limxx517 CSEL-student  1350 Mar 11 19:18 commands.h
```

#### Multiple pipes
```
[4061-shell]/home/limxx517/Documents/file-system-utilities $ ls -l | grep commands | grep 1350
-rw------- 1 limxx517 CSEL-student  1350 Mar 11 19:18 commands.h
```

### Testing output redirection

#### Truncate
```
[4061-shell]/home/limxx517/Documents/file-system-utilities $ ps > output.txt
[4061-shell]/home/limxx517/Documents/file-system-utilities $ cat output.txt
   PID TTY          TIME CMD
 31820 pts/10   00:00:00 bash
 39604 pts/10   00:00:00 shell
 40142 pts/10   00:00:00 ps
```

#### Append
```
[4061-shell]/home/limxx517/Documents/file-system-utilities $ echo "Testing append" >> output.txt
[4061-shell]/home/limxx517/Documents/file-system-utilities $ cat output.txt
   PID TTY          TIME CMD
 31820 pts/10   00:00:00 bash
 39604 pts/10   00:00:00 shell
 40142 pts/10   00:00:00 ps
"Testing append"
```

### Chaining pipes with output redirection
```
[4061-shell]/home/limxx517/Documents/file-system-utilities $ cat output.txt | grep pts > output2.txt
[4061-shell]/home/limxx517/Documents/file-system-utilities $ cat output2.txt
 31820 pts/10   00:00:00 bash
 39604 pts/10   00:00:00 shell
 40142 pts/10   00:00:00 ps
```

### Exiting the shell
```
[4061-shell]/home/limxx517/Documents/file-system-utilities $ exit
Exiting shell
limxx517@csel-vole-15:/home/limxx517/Documents/file-system-utilities $
```
