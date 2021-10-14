#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>

int const n  = 100;  //Примем 100 за максимальное количество символов в строке
int const n1 = 10;   //Предполагается, что в первой строке находится число с не более чем десятью знаками

void Split (char* string, char* delimiters, char*** tokens, int* tokensCount);

int main ()
{
    char str1[n1];
    int fp = open ("file_text", O_RDONLY);
    int num = read (fp, str1, n1);

    int number_of_string = atoi (str1);

    close (fp);

    char str[n * (number_of_string + 1)];
    fp = open ("file_text", O_RDONLY);
    num = read (fp, str, n * (number_of_string + 1));

    int arr[number_of_string + 1];  //Номера символов, означающих переход на новую строку
    int count = 0;

    for (int i = 0; i < num; i++)
        if (str[i] == '\n')
        {
            arr[count] = i;
            count++;
        }

    char** strings = (char**)calloc(number_of_string, sizeof (char*));

    for (int i = 0; i < number_of_string; i++)  //Разбиение на строки
    {
        strings[i] = (char*)calloc(arr[i + 1] - arr[i], sizeof (char));
        int count = 0;
        for (int j = arr[i] + 1; j < arr[i + 1]; j++)
        {
            strings[i][count] = str[j];
            count++;
        }
    }

    char*   delimiters  = " ";
    char**  tokens      = NULL;
    int     tokensCount = 0;
    int* time = (int*)calloc(number_of_string, sizeof (int));
    int cpids[number_of_string];    //Массив ID дочерних процессов
    int statusCpids[number_of_string];  //Массив статусов, с которыми завершились процессы

    for (int i = 0; i < number_of_string; i++)
    {
        Split (strings[i], delimiters, &tokens, &tokensCount);

        time[i] = atoi (tokens[tokensCount - 1]);
        tokens[tokensCount - 1] = NULL;

        int first_pipe[2] = {0};

        if (pipe (first_pipe) < 0)
        {
            printf ("Can't create pipe\n");
            exit (-1);
        }

        pid_t pid1 = fork();

        if (pid1 < 0)
        {
            printf ("Can't fork child\n");
            exit (-1);
        }

        else if (pid1 == 0)
        {
            close (first_pipe[0]);

            int cpid = getpid();
            int size = write (first_pipe[1], &cpid, sizeof (cpid));
            if (size != sizeof (cpid))
            {
                printf ("Can't write all cpid\n");
                exit (-1);
            }

            close (first_pipe[1]);

            if (i == 0)
                sleep (time[i]);

            else
                sleep (time[i] - time[i - 1]);

//            sleep (7);
            execvp (tokens[0], tokens);

            printf ("exit pid1\n");
            exit (0);
        }

        else
        {
            close (first_pipe[1]);

            int size = read (first_pipe[0], &cpids[i], sizeof (int));
            if (size < sizeof (int))
            {
                printf ("Can't read cpid\n");
                exit (-1);
            }

            close (first_pipe[0]);

            pid_t pid2 = fork();

            if (pid2 < 0)
            {
                printf ("Can't fork child\n");
                exit (-1);
            }

            else if (pid2 == 0)
            {
                if (i == 0)
                    sleep (time[i] + 5);

                else
                    sleep (time[i] - time[i - 1] + 5);

                kill (cpids[i], SIGTERM);

                exit(0);
            }

            waitpid (cpids[i], &statusCpids[i], 0);
            free (tokens);
            tokens = NULL;
            tokensCount = 0;
        }
    }

    printf ("\n");
    for (int i = 0; i < number_of_string; i++)
        printf ("N: %d, Pid: %d, Status: %d\n", i, cpids[i], statusCpids[i]);

    free (time);
    for (int i = 0; i < number_of_string; i++)
        free (strings[i]);
    free (strings);

    return 0;
}

void Split (char* string, char* delimiters, char*** tokens, int* tokensCount)
{
    char *str = strtok (string, delimiters);

    if (str == NULL)
    {
        printf ("Can't split string\n");
        return;
    }

    while (str != NULL)
    {
        (*tokensCount)++;
        *tokens = (char**)realloc(*tokens, *tokensCount * sizeof (char*));
        (*tokens) [*tokensCount - 1] = str;
        str = strtok (NULL, delimiters);
    }
}
