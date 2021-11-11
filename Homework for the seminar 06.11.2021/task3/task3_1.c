#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

const int N = 3;
int semid;
int msqid;

void *mythread (void *arg);
void makeop (int semid, int n);
void KillSem (char* file);

int main()
{
    char pathname[] = "09-1a.c";
    char pathnamesem[] = "08-1a.c";

    key_t key, keysem;

    int i,len;

    struct mymsgbuf1
    {
        long mtype;
        struct
        {
            int a, b, pid;
        } info;
    } InputMsg;

    KillSem (pathnamesem);

    if ((key = ftok (pathname,0)) < 0)
    {
        printf ("Can\'t generate key\n");
        exit (-1);
    }

    if ((keysem = ftok (pathnamesem, 0)) < 0)
    {
        printf ("Can\'t generate key\n");
        exit (-1);
    }

    if ((msqid = msgget (key, 0666 | IPC_CREAT)) < 0)
    {
        printf ("Can\'t get msqid\n");
        exit (-1);
    }

    if ((semid = semget (keysem, 1, 0666 | IPC_CREAT)) < 0)
    {
        printf ("Can\'t get semid\n");
        exit (-1);
    }

    makeop (semid, N);

    int maxlen;
    pthread_t thid, mythid;
    int result, counter = 0;
    struct mymsgbuf1* arg = (struct mymsgbuf1*)calloc(160, sizeof (struct mymsgbuf1));

    while (1)
    {
        maxlen = sizeof (InputMsg.info);

        if (len = msgrcv (msqid, (struct msgbuf1 *) &arg[counter], maxlen, 1, 0) < 0)
        {
            printf ("Can\'t receive message from queue\n");
            exit (-1);
        }

        result = pthread_create (&thid, (pthread_attr_t *)NULL, mythread, &arg[counter]);

        if (counter < 149)        
            counter++;
    }

    return 0;
} 

void *mythread (void *arg)
{
    struct mymsgbuf1
    {
        long mtype;
            struct
        {
            int a, b, pid;
        } info;
    };

    struct mymsgbuf1 dummy = *((struct mymsgbuf1*)arg);

    int len;

    makeop (semid, -1);

    struct mymsgbuf2
    {
        long mtype;
        struct
        {
            int result;
            float finfo;
        } info;
    } OutputMsg;

    OutputMsg.mtype = dummy.info.pid;
    OutputMsg.info.result = (dummy.info.a) * (dummy.info.b);
    len = sizeof (OutputMsg.info);

    if (msgsnd (msqid, (struct msgbuf2 *) &OutputMsg, len, 0) < 0)
    {
        printf ("Can\'t send message to queue\n");
        msgctl (msqid, IPC_RMID, (struct msqid_ds *) NULL);
        exit (-1);
    }

    makeop (semid, 1);
}

void makeop (int semid, int n)
{
    struct sembuf mybuf;

    mybuf.sem_op  = n;
    mybuf.sem_flg = 0;
    mybuf.sem_num = 0;

    if (semop (semid, &mybuf, 1) < 0)
    {
        printf ("Can\'t wait for condition\n");
        exit (-1);
    }
}

void KillSem (char* file)
{
    int keysem, semid;

    if ((keysem = ftok (file, 0)) < 0)
    {
        printf ("Can\'t generate key\n");
        exit (-1);
    }

    semid = semget (keysem, 1, 0);

    if (semid >= 0)
        semctl (semid, 0, IPC_RMID, 0);
}
