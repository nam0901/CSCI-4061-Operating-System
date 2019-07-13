# Inter-process Communication Project
## Contributions
### Jan-Wei Lim (limxx517)
- Wrote code for thread syncronization functionality
- Bug fixed memory leak issues

### Nam Ho (hoxxx395)
- Bug fixed memory leak issues
- Ran test cases  

### Jacob Bandyk (bandy036)
- Ran test cases
- Created README


## Overview
The focus of this project is to use mutex to synchronize concurrent linked list operations.  These operations run
through a text file and represent the file as nodes of a linked list which is then printed to an output file with information used by the multithreaded approach.

## Running the project

### Clone the project
Clone the project to your local via HTTPS or SSH :
```
git clone https://github.umn.edu/limxx517/synchronization.git
```

### Compile the project
To compile all source files in this project, just execute the following command :
```
make all
```

### Executing the project

First, make sure you have the name of the text file you would like to run the program on.

Then to execute do the following command:
```
./tlist filename thread-count
```
Where `filename` is the name of the file you would like to run the progam on and `thread-count` is an integer between 1 and 16 which specifies the number of threads.

An example would be as follows:
```
./tlist test.txt 8
```
Upon termination of the program, the output will be in the file `copy.txt` with the formatting being:
```
sequence number, line number, content
```
