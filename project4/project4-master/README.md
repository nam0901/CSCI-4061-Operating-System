# Inter-process Communication Project
## Contributions
### Jan-Wei Lim (limxx517)
- Worked Server implementation 
- Set up a shared memory segment for communication protocol

### Nam Ho (hoxxx395)
- Worked on client implementation
- Ran test cases  

### Jacob Bandyk (bandy036)
- Worked on client implementation with shared memory segment


## Overview
The main focus of this project is to implement a server program and two client programs in which the server can communicate 
privately to each client. In addition, the server can broadcast a message to both clients. 
To set up the communcation, the project needs to use message queues. Furthermore, it uses shared memory for passing the common key. 

## Running the project

### Clone the project
Clone the project to your local via HTTPS or SSH :
```
git clone https://github.umn.edu/bandy036/project4.git
```

### Compile the project
To compile all source files in this project, just execute the following command :
```
make all
```

### Executing the project

First we run the server program...
```
./server
```
Then, depending on the choice, in a separate terminal run the appropriate client:

a) Send and Receive messages privately from Client 1
```
./client1
```

b) Send and Receive messages privately from Client 2

```
./client2
```

c) Send the broadcase message, we can run either client and run them simutaneously each terminal
```
./client1
```


```
./client2
```


