#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    int msqid;

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

    maxlen = sizeof (mybuf.info);

    if (len = msgrcv (msqid, (struct msgbuf *) &mybuf, maxlen, 1, 0) < 0)
    {
        printf ("Can\'t receive message from queue\n");
        exit (-1);
    }

    int F = 1;

    for (int i = 2; i < (mybuf.info.sinfo + 1); i++)
        F = F * i;

    mybuf.mtype = 2;
    mybuf.info.sinfo = F;
    len = sizeof (mybuf.info);

    if (msgsnd (msqid, (struct msgbuf *) &mybuf, len, 0) < 0)
    {
        printf ("Can\'t send message to queue\n");
        msgctl (msqid, IPC_RMID, (struct msqid_ds *) NULL);
        exit (-1);
    }

    return 0;
}
