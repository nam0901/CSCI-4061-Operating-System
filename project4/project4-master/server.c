#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <unistd.h>
#include <errno.h>

#define BUFSIZE 1024
#define SHM_SIZE 1024
#define MSG_SIZE 1024
#define CLIENT_1_SHM_KEY 100
#define CLIENT_1_MSG_QUEUE_KEY 100
#define CLIENT_2_SHM_KEY 200
#define CLIENT_2_MSG_QUEUE_KEY 200

static int shm_id1, shm_id2;
static void *shm_data1, *shm_data2;
static int msgq_id1, msgq_id2;

enum MenuOptions {
	EXIT_OPTION,
	CLIENT_1_SEND_OPTION,
	CLIENT_2_SEND_OPTION,
	BROADCAST_OPTION
};

enum MessageOptions {
	EXIT_MSG = 1,
	PRIVATE_MSG,
	BROADCAST_MSG
};

void init() {
	// create a shared segment with client 1
	if((shm_id1 = shmget(CLIENT_1_SHM_KEY, SHM_SIZE, 0666 | IPC_CREAT)) == -1) {
		perror("shmget");
		exit(1);
	}

	// attach the created segment to memory
	if((shm_data1 = shmat(shm_id1, NULL, 0)) == (void*) -1) {
		perror("shmat");
		exit(1);
	}

	// write the message queue key to buffer
	char client1_msgq_key[4];
	sprintf(client1_msgq_key, "%d", CLIENT_1_MSG_QUEUE_KEY);
	strncpy(shm_data1, client1_msgq_key, SHM_SIZE);

	// open a message queue
	if((msgq_id1 = msgget(CLIENT_1_MSG_QUEUE_KEY, IPC_CREAT | 0666)) == -1) {
		perror("msgget");
		exit(1);
	}

	// repeat for client2
	if((shm_id2 = shmget(CLIENT_2_SHM_KEY, SHM_SIZE, IPC_CREAT | 0666)) == -1) {
		perror("shmget");
		exit(1);
	}

	if((shm_data2 = shmat(shm_id2, NULL, 0)) == (void*) -1) {
		perror("shmat");
		exit(1);
	}

	char client2_msgq_key[4];
	sprintf(client2_msgq_key, "%d", CLIENT_2_MSG_QUEUE_KEY);
	strncpy(shm_data2, client2_msgq_key, SHM_SIZE);

	if((msgq_id2 = msgget(CLIENT_2_MSG_QUEUE_KEY, IPC_CREAT | 0666)) == -1) {
		perror("msgget");
		exit(1);
	}
}

// send : sends a private message to client
void send(int client) {
	printf("Write Message: ");
	fflush(stdout);

	// read from stdin
	char buf[BUFSIZE];
	ssize_t bytes_read = read(STDIN_FILENO, buf, sizeof(buf) - 1);
	if(bytes_read < 0) {
		perror("read");
		exit(1);
	}
	buf[bytes_read] = '\0';

	// create the message buffer
	struct msg_buffer {
		long msg_type;
		char msg_text[MSG_SIZE];
	} message;

	// populate the message buffer
	message.msg_type = PRIVATE_MSG;
	strncpy(message.msg_text, buf, MSG_SIZE);

	int msgq_id = client == 1 ? msgq_id1 : msgq_id2;

	// send the message
	if(msgsnd(msgq_id, &message, sizeof(message), 0) == -1) {
		perror("msgsnd");
		exit(1);
	}

	// wait for response
	if(msgrcv(msgq_id, &message, sizeof(message), 0, 0) == -1) {
		perror("msgrcv");
		exit(1);
	}

	// display the response
	printf("\nMessage Received: %s", message.msg_text);
	fflush(stdout);
}

// broadcast : Sends the message to both clients to broadcast
void broadcast() {
	printf("Write Message to broadcast: ");
	fflush(stdout);

	// read from stdin
	char buf[BUFSIZE];
	ssize_t bytes_read = read(STDIN_FILENO, buf, sizeof(buf) - 1);
	if(bytes_read < 0) {
		perror("read");
		exit(1);
	}
	buf[bytes_read] = '\0';

	// create the message buffer
	struct msg_buffer {
		long msg_type;
		char msg_text[MSG_SIZE];
	} message;

	// populate the message buffer
	message.msg_type = BROADCAST_MSG;
	strncpy(message.msg_text, buf, MSG_SIZE);

	// send the message to both clients
	if(msgsnd(msgq_id1, &message, sizeof(message), 0) == -1) {
		perror("msgsnd");
		exit(1);
	}
	if(msgsnd(msgq_id2, &message, sizeof(message), 0) == -1) {
		perror("msgsnd");
		exit(1);
	}

	return;
}

// terminate : Actions to take before terminating the server
void terminate() {
	struct msg_buffer {
		long msg_type;
		char msg_text[1];
	} message;

	message.msg_type = EXIT_MSG;
	// no need for an actual message text here

	// notify both clients about the termination
	if(msgsnd(msgq_id1, &message, sizeof(message), 0) == -1) {
		perror("msgsnd");
		exit(1);
	}
	if(msgsnd(msgq_id2, &message, sizeof(message), 0) == -1) {
		perror("msgsnd");
		exit(1);
	}

	// detach from shared memory spaces of both clients
	if(shmdt(shm_data1) == -1) {
		perror("shmdt");
		exit(1);
	}
	if(shmdt(shm_data2) == -1) {
		perror("shmdt");
		exit(1);
	}

	// exit the process
	exit(0);
}

int main() {
	// initialize shared memory space, message queues
	init();

	// buffer to retrive input from stdin
	char buf[BUFSIZE];

	// option chosen by user
	int option_num;

	while (1) {
		printf("\nEnter 1 for client 1\n      2 for client 2\n      3 for broadcasting\n      0 to exit.\n\nEnter choice: ");
		fflush(stdout);

		ssize_t bytes_read = read(STDIN_FILENO, buf, sizeof(buf) - 1);
		if(bytes_read < 0) {
			perror("read");
			exit(1);
		}
		buf[bytes_read] = '\0';

		option_num = atoi(buf);
		//edge case
		if(option_num == 0) {
			if(strcmp(buf, "0\n") != 0) {
				option_num = -1;
			}
		}

		switch(option_num) {
			case EXIT_OPTION:
				terminate();

			case CLIENT_1_SEND_OPTION:
				send(1);
				break;

			case CLIENT_2_SEND_OPTION:
				send(2);
				break;

			case BROADCAST_OPTION:
				broadcast();
				break;

			default:
				printf("Invalid input. Enter 0, 1, 2, or 3.\n");
				fflush(stdout);
		}
	}

	return 0;
}
