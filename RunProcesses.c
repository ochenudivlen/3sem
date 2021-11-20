#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>

//Эта программа запускает все программы из данного файла

/*Пример содержимого считываемого файла
3
ls -al 6
pwd 3
echo Hello, world! 9
*/

int const n  = 128;

void Split (char* string, char* delimiters, char*** tokens, int* tokensCount);

int main ()
{
    FILE *fp;
    char str[n];

    fp = fopen ("file_text", "r");

    fgets (str, n, fp);
    int number_of_strings = atoi (str);

    char** strings = (char**)calloc(number_of_strings, sizeof (char*));
    for (int i = 0; i < number_of_strings; i++)
        strings[i] = (char*)calloc(n, sizeof (char));

    int count = 0;

    while (!feof (fp))
    {
        if (fgets (str, n, fp))
            strcpy (strings[count], str);

        count++;
    }

    char*   delimiters  = " ";
    char**  tokens      = NULL;
    int     tokensCount = 0;
    int* time           = (int*)calloc(number_of_strings, sizeof (int));
    int cpids[number_of_strings];
    int statusCpids[number_of_strings];

    for (int i = 0; i < number_of_strings; i++)
    {
        char* temp_str;
        strcpy (temp_str, strings[i]);

        Split (strings[i], delimiters, &tokens, &tokensCount);
        time[i] = atoi (tokens[tokensCount - 1]);

        strcpy (strings[i], temp_str);

        free (tokens);
        tokens = NULL;
        tokensCount = 0;
    }

    for (int i = 0; i < number_of_strings; i++)
        printf ("%s", strings[i]);

    for (int i = 0; i < (number_of_strings - 1); i++)
        for (int j = 1; j < (number_of_strings - i); j++)
            if (time[j] < time[j - 1])
            {
                char* temp_str = strings[j];
                strings[j] = strings[j - 1];
                strings[j - 1] = temp_str;

                int temp_time = time[j];
                time[j] = time[j - 1];
                time[j - 1] = temp_time;
            }

    fclose (fp);

    for (int i = 0; i < number_of_strings; i++)
    {
        Split (strings[i], delimiters, &tokens, &tokensCount);

        tokens[tokensCount - 1] = NULL;

        pid_t pid1 = fork();

        if (pid1 < 0)
        {
            printf ("Can't fork child\n");
            exit (-1);
        }

        else if (pid1 == 0)
        {
            if (i == 0)
                sleep (time[i]);

            else
                sleep (time[i] - time[i - 1]);

            execvp (tokens[0], tokens);

            printf ("exit pid1\n");
            exit (0);
        }

        else
        {
            cpids[i] = pid1;

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

    for (int i = 0; i < number_of_strings; i++)
    {
        printf ("N: %d, Pid: %d, Status: %d, ", i, cpids[i], statusCpids[i]);

        if (statusCpids[i] == 15)
            printf ("Stopped with kill\n");

        else
            printf ("Ended on its own or for another reason\n");
    }

    free (time);

    for (int i = 0; i < number_of_strings; i++)
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
