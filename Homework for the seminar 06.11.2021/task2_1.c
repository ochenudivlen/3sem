#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    int msqid, N;

    printf ("N: ");
    scanf ("%d", &N);

    char pathname[] = "09-1a.c";

    key_t key;

    int len, maxlen;

    struct mymsgbuf
    {
        long mtype;
            struct
        {
            int sinfo;
            float finfo;
        } info;
    } mybuf;

    if ((key = ftok (pathname,0)) < 0)
    {
        printf ("Can\'t generate key\n");
        exit (-1);
    }

    if ((msqid = msgget (key, 0666 | IPC_CREAT)) < 0)
    {
        printf ("Can\'t get msqid\n");
        exit (-1);
    }

    mybuf.mtype = 1;
    mybuf.info.sinfo = N;
    len = sizeof (mybuf.info);

    if (msgsnd (msqid, (struct msgbuf *) &mybuf, len, 0) < 0)
    {
        printf ("Can\'t send message to queue\n");
        msgctl (msqid, IPC_RMID, (struct msqid_ds *) NULL);
        exit (-1);
    }

    maxlen = sizeof (mybuf.info);

    if (len = msgrcv (msqid, (struct msgbuf *) &mybuf, maxlen, 2, 0) < 0)
    {
        printf ("Can\'t receive message from queue\n");
        exit (-1);
    }

    printf ("N! = %d\n", mybuf.info.sinfo);

    msgctl (msqid, IPC_RMID, (struct msqid_ds *) NULL);

    return 0;
} 
