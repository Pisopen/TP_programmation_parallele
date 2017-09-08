#include <stdio.h>
#include <sys/msg.h>

typedef struct {
	long int receiverid;
	long int senderid;
	char operation;
	
} msg;

int main() {
	int msgqueueid = msgget(IPC_PRIVATE, IPC_CREAT);
	msgctl(msgqueueid, IPC_RMID, NULL);
	printf("Hello, World!\n");
	return 0;
}