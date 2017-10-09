#include <sys/msg.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#define N 5

typedef struct {
	long id_rec;
	long id_em;
	int ope;
	long date;
} Msg;

pthread_t tProc[N];
int idfile;
long date_start;
pthread_mutex_t mutex;

Msg initMsg(long idr, long ide, int op){
  Msg message;
  message.id_rec=idr;
  message.id_em=ide;
  message.ope=op;
  return message;
}



void init_lock() {

	pthread_mutex_init(&mutex, NULL);
	idfile = msgget(IPC_PRIVATE, IPC_CREAT | 0666);
	date_start = time(NULL);
	printf("Id de la file de message: %d\n", idfile);
}


static void * envoyer_message(void* argv) {
	Msg* msg = (Msg*) argv;
	/*if(pthread_mutex_trylock(&mutex)==0){
	    printf("Unlocked");
	}*/
	//lock();
	msg->date = date_start++; //ecriture
	printf("Message envoyé du processus %ld au processus %ld à la date %ld\n", msg->id_em, msg->id_rec, msg->date);
	//unlock();
	msgsnd(idfile, msg, sizeof(msg), 0);

	return NULL;
}

int main() {
	init_lock();
	Msg* msg;
	
	for (int i = 1; i <= N; i++) {
		msg=malloc(sizeof(Msg));
		msg->id_rec = N - i + 1;
		msg->id_em = i;
		pthread_create(&tProc[i], NULL, envoyer_message , (void*) msg);
	}
	//showListe();
	for (int i = 0; i < N; i++) {
		pthread_join(tProc[i], NULL);
	}
	return 0;
}