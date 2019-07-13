# Virtual Memory Manager
## Contributions
### Jan-Wei Lim (limxx517)
- FIFO TLB implementation

### Nam Ho (hoxxx395)
- worked on fetch_physical_frame
- print_physical_address
### Jacob Bandyk (bandy036)
- worked on fetch_physical_frame
- LRU TLB implementation

## Overview
The main focus of this project is to implement a translation that translates virtual
addresses to physical addresses using two-level page tables (PTs) and a TLB (Translation
Lookaside Buffer) that implements the “First In First Out” policy for page replacement, as well as extra credit LRU "Least Recently Used" policy for page replacement.

## Running the project

### Clone the project
Clone the project to your local via HTTPS or SSH :
```
git clone https://github.umn.edu/bandy036/project3.git
```

### Compile the project
To compile all source files in this project, just execute the following command :
```
make all
```

### Executing the project
First cd into src directory
```
cd src
```
Then to run the FIFO policy...
```
./vmanager
```
To run the extra credit LRU policy
```
./vmanager -lru
```

The dump of the current TLB after the predefined tests written in `main.c` will be in `./bin/tlb_out.txt` and the physical frame for each virtual address will be output to the terminal

