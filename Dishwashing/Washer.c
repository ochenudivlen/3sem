#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <unistd.h>

//В этой программе описан процесс, олицетворяющий человека, моющего посуду. Реализацию производим с помощью очереди сообщений и семафора

const int N1 = 20;                                                          //Считаем, что больше 20 разновидностей посуды быть не может
const int N2 = 256;                                                         //Максимальная длина строки
const int TABLE_LIMIT = 4;                                                  //Максимальное количество предметов на столе
const int LAST_MESSAGE = 255;                                               //Тип посленего сообщения, которое мы отправим второму процессу в знак того, что вся посуда вымыта

int Reading (char* pathname, char*** dishes, int** vals);                   //С помощью этой функции будем производить чтение из фала
void makeop (int semid, int n);                                             //Изменение состояния семафора
void KillSem (char* file);                                                  //Удаление старого семафора

int main()
{
    char** dishes = NULL;                                                   //Массив с наименованиями посуды
    int* times = NULL;                                                      //Массив со значениями времени, которое понадобиться на мытье одной разновидности посуды
    char** queue_of_dishes = NULL;                                          //Массив с наименованиями грязной посуды, лежащей на столе (своеобразная очередь из наименований посуды)
    int* numbers = NULL;                                                    //Массив с количествами грязной посуды

    int number_of_dishes = Reading ("Washing", &dishes, &times);            //Количество наименований посуды

    char pathname[] = "09-1a.c";                                            //Файл, с помощью которого найдём ключ для очереди сообщений
    char pathnamesem[] = "08-1a.c";                                         //Файл, с помощью которого найдём ключ для семафоров
    int semid, msqid;                                                       //Ключи и идентификаторы семафора и очереди сообщений
    key_t key, keysem;

    struct mymsgbuf                                                         //Определение отправляемого в очередь сообщения
    {
        long mtype;
        char dishes[20];
    } OutputMsg;

    KillSem (pathnamesem);                                                  //Уничтожаем прошлый семафор

    if ((key = ftok (pathname,0)) < 0)                                      //Определение ключей и получение доступа к семафору и очереди сообщений
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

    makeop (semid, TABLE_LIMIT);                                            //Задаём начальное значение семафора

    int queue = Reading ("Dishes", &queue_of_dishes, &numbers);             //"Размер" очереди грязной посуды (то есть количество наименований грязной посуды (с повторениями))

    OutputMsg.mtype = 1;                                                    //Задаем тип сообщения
    int time = 0;                                                           //Здесь будет храниться время, требуемое на помывку некоторого типа посуды

    for (int i = 0; i < queue; i++)                                         //Проходим по очереди из грязной посуды
    {
        for (int j = 0; j < number_of_dishes; j++)                          //Определяем время, необходимое на помывку данного типа посуды
            if (strcmp (queue_of_dishes[i], dishes[j]) == 0)
                time = times[j];

        for (int j = 0; j < numbers[i]; j++)                                //Моем посуду, котрую встретили в очереди
        {
            makeop (semid, -1);                                             //Уменьшаем семафор в знако уменьшения количества свободного места на столе

            printf ("I'm washing %s now\n", queue_of_dishes[i]);            //Печатаем информацию о посуде, которую моем в данный момент времени
            sleep (time);       //Задержка времени

            strcpy (OutputMsg.dishes, queue_of_dishes[i]);                  //Зполняем информационное поле сообщения, которое передадим второму процессу
            int len = sizeof (OutputMsg.dishes) + 1;                        //Размер поля

            if (msgsnd (msqid, (struct mymsgbuf *) &OutputMsg, len, 0) < 0) //Отправляем сообщение
            {
                printf ("Can\'t send message to queue\n");
                msgctl (msqid, IPC_RMID, (struct msqid_ds *) NULL);
                exit (-1);
            }
        }
    }

    OutputMsg.mtype = LAST_MESSAGE;                                         //Далее отправим пустое сообщение в знак окончания очереди грязной посуды, его тип будет отличаться от предыдущих
    int len = 0;

    if (msgsnd(msqid, (struct mymsgbuf *) &OutputMsg, len, 0) < 0)
    {
        printf("Can\'t send message to queue\n");
        msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
        exit(-1);
    }

    for (int i = 0; i < number_of_dishes; i++)                              //Далее очищаем память, веделенную calloc
        free (dishes[i]);
    free (dishes);

    for (int i = 0; i < queue; i++)
        free (queue_of_dishes[i]);
    free (queue_of_dishes);

    free (times);

    free (numbers);

    return 0;
}

int Reading (char* pathname, char*** dishes, int** vals)                    //Чтение из фала
{
    char** strings = (char**)calloc(N1, sizeof (char*));                    //Массив строчек, которые мы прочитаем
    for (int i = 0; i < N1; i++)
        strings[i] = (char*)calloc(N2, sizeof (char));

    FILE *f = fopen (pathname, "r");

    int temp = 0;                                                           //Здесь будет лежать количество прочитанных строк

    while (!feof(f))                                                        //Читаем, пока не наступит конец строки
    {
        fscanf (f, "%s", strings[temp]);
        temp++;
    }

    fclose (f);

    temp = temp / 2;                                                        //Делим количество строк на два, так как мы кладём наименования посуды и время, которое им нужно (или ее количество), в разные строки массива (получается в два раза больше, чем нужно)

    *dishes = (char**)calloc(temp, sizeof (char*));                         //Выделяем память для отдельного массива наименований
    for (int i = 0; i < temp; i++)
        (*dishes)[i] = (char*)calloc(N2, sizeof (char));

    *vals = (int*)calloc(temp, sizeof (int));                               //Выделяем пямять для отдельного массива времен (или количеств посуды)

    int count = 0;                                                          //Счётчик для удобства "разделения" массива строк на массивы наименований посуды и времен (или количеств посуды)

    for (int i = 0; i < (temp * 2); i++)                                    //Формируем массивы наименований и времен (количеств посуды)
    {
        if (i % 2 == 0)
            strncpy ((*dishes)[count], strings[i], strlen (strings[i]) - 1);//Убираем двоеточие, содержащееся в строке с наименованием
        else
        {
            (*vals)[count] = atoi (strings[i]);                             //Преобразуем строку в число
            count++;
        }
    }

    for (int i = 0; i < N1; i++)                                            //Очищаем выделеную для строк память
        free (strings[i]);
    free (strings);

    return temp;
}

void makeop (int semid, int n)                                              //Изменяем значение семафора
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

void KillSem (char* file)                                                   //Убиваем предыдущий семафор
{
    int keysem, semid;

    if ((keysem = ftok (file, 0)) < 0)                                      //Достаем ключ для массива семафоров
    {
        printf ("Can\'t generate key\n");
        exit (-1);
    }

    semid = semget (keysem, 1, 0);                                          //Получаем доступ к массиву семафоров

    if (semid >= 0)                                                         //Удаляем, если он существовал
        semctl (semid, 0, IPC_RMID, 0);
}
