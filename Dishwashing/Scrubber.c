#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <unistd.h>

//В этой программе описан процесс, олицетворяющий человека, вытирающего посуду. Реализацию производим с помощью очереди сообщений и семафора. Здесь нет комментариев к частям кода, идентичным процессу-мойщику

const int N1 = 20;
const int N2 = 256;
const int LAST_MESSAGE = 255;

int Reading (char* pathname, char*** dishes, int** vals);
void makeop (int semid, int n);
void KillSem (char* file);

int main()
{
    char** dishes = NULL;
    int* times = NULL;

    int number_of_dishes = Reading ("Wiping", &dishes, &times);

    char pathname[] = "09-1a.c";
    char pathnamesem[] = "08-1a.c";
    int semid, msqid;

    key_t key, keysem;

    struct mymsgbuf
    {
        long mtype;
        char dishes[20];
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

    if ((semid = semget (keysem, 1, 0666 | IPC_CREAT)) < 0)
    {
        printf ("Can\'t get semid\n");
        exit (-1);
    }

    int time = 0;
    int len = 0;

    while(1)                                                                        //В бесконечном цикле принимаем сообщения, отправляемые процессом-мойщиком
    {
        int maxlen = sizeof (InputMsg.dishes) + 1;

        if (len = msgrcv(msqid, (struct mymsgbuf *) &InputMsg, maxlen, 0, 0) < 0)   //Читаем все сообщения подряд
        {
            printf("Can\'t receive message from queue\n");
            exit(-1);
        }

        if (InputMsg.mtype == LAST_MESSAGE)                                         //Если это последнее сообщение - выходим из программы
        {
            msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
            exit(0);
        }

        for (int j = 0; j < number_of_dishes; j++)                                  //Ищем время, необходимое для вытирания данного типа посуды
            if (strcmp (InputMsg.dishes, dishes[j]) == 0)
                time = times[j];

        makeop (semid, 1);                                                          //Повышаем значение семафора в знак того, что мы освободили одно место на столе

        printf ("I'm wiping %s now\n", InputMsg.dishes);                            //Печатаем информацию о том, что начали мыть посуду
        sleep (time);                                                               //Задержка времени
    }

    for (int i = 0; i < number_of_dishes; i++)
        free (dishes[i]);
    free (dishes);

    free (times);

    return 0;
}

int Reading (char* pathname, char*** dishes, int** vals)                            //Чтение из файла
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

void makeop (int semid, int n)                                                      //Изменение состояния семафора
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

void KillSem (char* file)                                                           //Удаление предыдущего семафора
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
