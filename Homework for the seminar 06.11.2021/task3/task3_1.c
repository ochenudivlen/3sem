#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

const int N = 2;

void *mythread (void *arg);

int main()
{
    int msqid, semid;

    char pathname[] = "09-1a.c";
    char pathnamesem[] = "08-1a.c";

    key_t key, keysem;

    int i,len;

    struct sembuf mybuf;

    struct mymsgbuf1
    {
        long mtype;
        struct
        {
            int a, b, pid;
        } info;
    } InputMsg;

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

    if ((semid = semget (key, 1, 0666 | IPC_CREAT)) < 0)
    {
        printf ("Can\'t get semid\n");
        exit (-1);
    }

    mybuf.sem_op  = N;
    mybuf.sem_flg = 0;
    mybuf.sem_num = 0;

    if (semop (semid, &mybuf, 1) < 0)
    {
        printf ("Can\'t wait for condition\n");
        exit (-1);
    }

    int maxlen;
    pthread_t thid, mythid;
    int result;

    while (1)
    {
        maxlen = sizeof (InputMsg.info);

        if (len = msgrcv (msqid, (struct msgbuf1 *) &InputMsg, maxlen, 0, 0) < 0)
        {
            printf ("Can\'t receive message from queue\n");
            exit (-1);
        }

        struct mymsgbuf1 arg = InputMsg;

        result = pthread_create (&thid, (pthread_attr_t *)NULL, mythread, &arg);
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
    struct sembuf mybuf;
    int semid, len, msqid;

    mybuf.sem_op  = -1;
    mybuf.sem_flg =  0;
    mybuf.sem_num =  0;

    if (semop (semid, &mybuf, 1) < 0)
    {
        printf ("Can\'t wait for condition\n");
        exit (-1);
    }

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

    sleep (5);

    if (msgsnd (msqid, (struct msgbuf2 *) &OutputMsg, len, 0) < 0)
    {
        printf ("Can\'t send message to queue\n");
        msgctl (msqid, IPC_RMID, (struct msqid_ds *) NULL);
        exit (-1);
    }

    mybuf.sem_op  = 1;
    mybuf.sem_flg = 0;
    mybuf.sem_num = 0;

    if (semop (semid, &mybuf, 1) < 0)
    {
        printf ("Can\'t wait for condition\n");
        exit (-1);
    }
}
