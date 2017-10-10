#include <sys/msg.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <sys/ipc.h>
#define N 5

struct msqid_ds buf;
typedef struct {
	long id_rec;
	long id_em;
	int ope;
	long date;
} Msg;

pthread_t tProc[N];
int msqid;
long date_start;
key_t k;
Msg* msg;

void init_lock() {
	k = ftok("projet", 0);
	if (k == -1)
	{
		printf( "erreur de creation de la cle IPC\n");
		exit (0);
	}

	printf ("Création d'une file de messages..\n");
	msqid = msgget (k , 0666 + IPC_CREAT + IPC_EXCL);
	if (msqid == -1)
	{
		printf ("La file de message existe déjà, ouverture...\n");
		msqid = msgget (k , 0666 + IPC_CREAT);
		if (msqid == -1)
		{
			printf( "erreur de création ou d'ouverture de la file de messages \n");
			exit(0);
		}
	}
	date_start = time(NULL);
	printf("Id de la file de message: %d\n", msqid);
}

void envoyer_message(Msg* msg) {
	msg->date = date_start++;
	printf("Message envoyé du processus %ld au processus %ld à la date %ld\n", msg->id_em, msg->id_rec, msg->date);
	msgsnd(msqid, msg, sizeof(msg), 0);
}

void demande_exclusion(Msg* msg) {
	printf("Message pour: %ld de: %ld a la date: %ld\n", msg->id_rec, msg->id_em, msg->date );
}

void reponse_exclusion(Msg* msg) {
	msg->date = date_start++;
	msgsnd(msqid, msg, sizeof(msg), 0);
}

void attendre_message() {
	long rec = msg->id_em;
	msgctl(msqid, IPC_STAT, &buf);
	for (int i = 0; i < buf.msg_qnum; ++i)
	{
		msgrcv(msqid, msg, sizeof(msg), 0, IPC_NOWAIT);
		msg->date = date_start++;
		if (msg->id_rec == rec)
		{
			demande_exclusion(msg);
		}
		else {
			reponse_exclusion(msg);
		}
	}
}

static void * communication(void* argv) {
	Msg* msg = (Msg*) argv;
	envoyer_message(msg);
	attendre_message();
	return NULL;
}

int main() {
	init_lock();


	for (int i = 1; i <= N; i++) {
		msg = malloc(sizeof(Msg));
		printf("Proc src : ");
		scanf("%ld", &msg->id_em);
		printf("Proc dest : ");
		scanf("%ld", &msg->id_rec);
		pthread_create(&tProc[i], NULL, communication , (void*) msg);
		pthread_join(tProc[i], NULL);
	}
	return 0;
}
