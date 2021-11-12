#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <unistd.h>

const int N1 = 20;      //Считаем, что больше 20 разновидностей посуды быть не может
const int N2 = 256;
const int TABLE_LIMIT = 4;

int Reading (char* pathname, char*** dishes, int** vals);
void makeop (int semid, int n);
void KillSem (char* file);

int main()
{
    char** dishes = NULL;
    int* times = NULL;
    char** queue_of_dishes = NULL;
    int* numbers = NULL;

    int number_of_dishes = Reading ("Washing", &dishes, &times);

    char pathname[] = "09-1a.c";
    char pathnamesem[] = "08-1a.c";
    int semid, msqid;

    key_t key, keysem;
    int i,len;

    struct mymsgbuf
    {
        long mtype;
        char dishes[20];
    }InputMsg, OutputMsg;

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

    makeop (semid, TABLE_LIMIT);

    int queue = Reading ("Dishes", &queue_of_dishes, &numbers);

    for (i = 0; i < queue; i++)
    {
        for (j = 0; j < numbers[i]; j++)
        {
//            strcmp ...
        }
    }

    for (int i = 0; i < number_of_dishes; i++)
        free (dishes[i]);
    free (dishes);

    for (int i = 0; i < queue; i++)
        free (queue_of_dishes[i]);
    free (queue_of_dishes);

    free (times);

    free (numbers);

    return 0;
}

int Reading (char* pathname, char*** dishes, int** vals)
{
    char** strings = (char**)calloc(N1, sizeof (char*));
    for (int i = 0; i < N1; i++)
        strings[i] = (char*)calloc(N2, sizeof (char));

    FILE *f = fopen (pathname, "r");

    int temp = 0;

    while (!feof(f))
    {
        fscanf (f, "%s", strings[temp]);
        temp++;
    }

    fclose (f);

    temp = temp / 2;

    *dishes = (char**)calloc(temp, sizeof (char*));
    for (int i = 0; i < temp; i++)
        (*dishes)[i] = (char*)calloc(N2, sizeof (char));

    *vals = (int*)calloc(temp, sizeof (int));

    int count = 0;

    for (int i = 0; i < (temp * 2); i++)
    {
        if (i % 2 == 0)
            strncpy ((*dishes)[count], strings[i], strlen (strings[i]) - 1);
        else
        {
            (*vals)[count] = atoi (strings[i]);
            count++;
        }
    }

    for (int i = 0; i < N1; i++)
        free (strings[i]);
    free (strings);

    return temp;
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
