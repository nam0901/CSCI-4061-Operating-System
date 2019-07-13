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
#define CLIENT_2_SHM_KEY 200

static int shm_id2;
static void *shm_data2;
static int msgq_id2;


enum MessageOptions {
	EXIT_MSG = 1,
	PRIVATE_MSG,
	BROADCAST_MSG
};

void init() {
	// create a shared segment with client 2
	if((shm_id2 = shmget(CLIENT_2_SHM_KEY, SHM_SIZE, 0666 | IPC_CREAT)) == -1) {
		perror("shmget");
		exit(1);
	}

	// attach the created segment to memory
	if((shm_data2 = shmat(shm_id2, NULL, 0)) == (void*) -1) {
		perror("shmat");
		exit(1);
	}
  // get msgq_id2 from shared memory
  msgq_id2 = atoi(shm_data2);
	// open a message queue
	if((msgq_id2 = msgget(msgq_id2, 0666 | IPC_CREAT)) == -1) {
		perror("msgget");
		exit(1);
	}
}

// terminate : Actions to take before terminating the server
void terminate() {

	// detach from shared memory spaces of both clients
	if(shmdt(shm_data2) == -1) {
		perror("shmdt");
		exit(1);
	}

	// mark both shared spaces for deletion
	if(shmctl(shm_id2, IPC_RMID, NULL) == -1) {
		perror("shmctl");
		exit(1);
	}

	// remove both message queues
	if(msgctl(msgq_id2, IPC_RMID, NULL) == -1) {
		perror("msgctl");
		exit(1);
	}
	exit(0);
}

int main() {
	// initialize shared memory space, message queues
	init();

	// buffer to retrive input from stdin
	char buf[BUFSIZE];
	// create the message buffer
	struct msg_buffer {
		long msg_type;
		char msg_text[MSG_SIZE];
	} message;

	while (1) {

		if(msgrcv(msgq_id2, &message, sizeof(message), 0, MSG_NOERROR) == -1) {
			perror("msgrcv");
			exit(1);
		}
		switch (message.msg_type) {
			case EXIT_MSG:
				printf("Server closed connection.\n");
				terminate();
				break;
			case PRIVATE_MSG:
				printf("\nMessage Received : %s\n", message.msg_text);
				printf("Write Response : ");
				fflush(stdout);
				ssize_t bytes_read = read(STDIN_FILENO, buf, sizeof(buf) - 1);
				if(bytes_read < 0) {
					perror("read");
					exit(1);
				}
				buf[bytes_read] = '\0';
				strncpy(message.msg_text, buf, MSG_SIZE);
				// send the message
				if(msgsnd(msgq_id2, &message, sizeof(message), 0) == -1) {
					perror("msgsnd");
					exit(1);
				}
				break;
			case BROADCAST_MSG:
				printf("\nMessage Received : %s", message.msg_text);
				fflush(stdout);
				break;
		};
	}
	return 0;
}
