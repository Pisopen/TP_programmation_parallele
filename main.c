#include <stdio.h>
#include <sys/msg.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>

#define CHILDREN 5

typedef struct {
	long int receiverid;
	long int senderid;
	char operation;
	time_t date;
} msg;

void child(long int childid, int qid) {
	time_t date;
	int i;
	msg msg1;
	
	msg1.senderid = qid;
	for (i = 0; i < CHILDREN; ++i) {
		msg1.receiverid = i;
		msg1.operation = 0;
		msg1.date = time(NULL);
		msgsnd(qid, &msg1, sizeof(msg), 0);
	}
	
	struct msqid_ds buf;
	msgctl(qid, IPC_STAT, &buf);
	
	for (i = 0; i < buf.msg_qnum; ++i) {
	}
}

int main() {
	int i, qid = msgget(IPC_PRIVATE, IPC_CREAT);
	
	for (i = 0; i < CHILDREN; ++i) {
		if (!fork()) {
			child(i, qid);
			return 0;
		}
	}
	
	for (i = 0; i < CHILDREN; ++i) {
		wait(NULL);
	}
	
	msgctl(qid, IPC_RMID, NULL);
	return 0;
}