#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int main()
{
    int msqid;

    char pathname[] = "09-1a.c";

    key_t key;

    int i,len;

    struct mymsgbuf1
    {
        long mtype;
            struct
        {
            int a, b, pid;
        } info;
    } OutputMsg;

    struct mymsgbuf2
    {
        long mtype;
        int result;
    } InputMsg;

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

    int maxlen;

    for (i = 1; i <= 5; i++)
    {
        OutputMsg.mtype  =  getpid();
        OutputMsg.info.a = 10;
        OutputMsg.info.b =  i * 3;
        len = sizeof (OutputMsg.info);

        if (msgsnd (msqid, (struct mymsgbuf1 *) &OutputMsg, len, 0) < 0)
        {
            printf ("Can\'t send message to queue\n");
            msgctl (msqid, IPC_RMID, (struct msqid_ds *) NULL);
            exit (-1);
        }

//        sleep (7);

        maxlen = sizeof (InputMsg.result);

        if (len = msgrcv (msqid, (struct mymsgbuf2 *) &InputMsg, maxlen, getpid(), 0) < 0)
        {
            printf ("Can\'t receive message from queue\n");
            exit (-1);
        }

        printf ("Result: %d\n", InputMsg.result);
    }

    return 0;
} 
