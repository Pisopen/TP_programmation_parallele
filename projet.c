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
pthread_mutex_t mutex;
pthread_cond_t condition = PTHREAD_COND_INITIALIZER;
key_t k;

void init_lock() {

	pthread_mutex_init(&mutex, NULL);
	k = ftok("key", 0);
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


static void * communication(void* argv) {
	long em;
	long rec;
	char ans;

	Msg* msg = (Msg*) argv;
	printf("Proc: \n");
	scanf("%ld", &em);
	msg->id_em = em;

	printf("Envoyer ou Recevoir (e ou r) \n");
	printf("test1\n");
	scanf("%c", &ans);
	if (ans == 'e')
	{
		pthread_mutex_lock(&mutex);
		pthread_cond_wait (&condition, &mutex);
		printf("Proc pour message: \n");
		scanf("%ld", &rec);
		msg->id_rec = rec;
		msg->date = date_start++;
		printf("Message envoyé du processus %ld au processus %ld à la date %ld\n", msg->id_em, msg->id_rec, msg->date);
		msgsnd(msqid, msg, sizeof(msg), 0);
		pthread_mutex_unlock(&mutex);
	}
	else if (ans == 'r')
	{
		pthread_mutex_lock(&mutex);
		pthread_cond_wait (&condition, &mutex);
		msgctl(msqid, IPC_STAT, &buf);
		for (int i = 0; i < buf.msg_qnum; ++i)
		{
			pthread_mutex_lock(&mutex);
			pthread_cond_wait (&condition, &mutex);
			msgrcv(msqid, msg, sizeof(msg), 0, IPC_NOWAIT);
			msg->date = date_start++;
			if (msg->id_rec == em)
			{
				pthread_mutex_lock(&mutex);
				pthread_cond_wait (&condition, &mutex);
				printf("Message pour:%ld de:%ld a la date:%ld\n", msg->id_rec, msg->id_em, msg->date );
				pthread_mutex_unlock(&mutex);
			}
			else {
				pthread_mutex_lock(&mutex);
				pthread_cond_wait (&condition, &mutex);
				msg->date = date_start++;
				msgsnd(msqid, msg, sizeof(msg), 0);
				pthread_mutex_unlock(&mutex);
			}
			pthread_mutex_unlock(&mutex);
		}
		pthread_mutex_unlock(&mutex);

	}

	return NULL;
}

int main() {
	init_lock();
	Msg* msg;

	for (int i = 1; i <= N; i++) {
		msg = malloc(sizeof(Msg));
		pthread_create(&tProc[i], NULL, communication , (void*) msg);
		pthread_join(tProc[i], NULL);
	}
	/*for (int i = 1; i <= N; i++) {
		pthread_join(tProc[i], NULL);
	}*/
	return 0;
}
