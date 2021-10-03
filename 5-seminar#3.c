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

    int arr[number_of_string + 1];
    int count = 0;

    for (int i = 0; i < num; i++)
        if (str[i] == '\n')
        {
            arr[count] = i;
            count++;
        }

    char** strings = (char**)calloc(number_of_string, sizeof (char*));

    for (int i = 0; i < number_of_string; i++)
    {
        strings[i] = (char*)calloc(arr[i + 1] - arr[i], sizeof (char));
        int count = 0;
        for (int j = arr[i] + 1; j < arr[i + 1]; j++)
        {
            strings[i][count] = str[j];
            count++;
        }
    }

    char*   delimiters   = " ";
    char**  tokens       = NULL;
    int     tokensCount  = 0;

    for (int i = 0; i < number_of_string; i++)
    {
        Split (strings[i], delimiters, &tokens, &tokensCount);

        tokens = (char**)realloc(tokens, (tokensCount + 1) * sizeof (char*));
        tokens [tokensCount] = NULL;

        struct timespec mt1, mt2;
        double time = 0;

        pid_t pid = fork();

        if (pid < 0)
        {
            printf ("Can\'t fork child\n");
            exit (-1);
        }
        else if (pid == 0)
        {
            sleep (3);

            execvp (tokens[0], tokens);

            exit (0);
        }
        else
        {
            clock_gettime (CLOCK_REALTIME, &mt1);

            wait (NULL);

            clock_gettime (CLOCK_REALTIME, &mt2);

            time = (mt2.tv_sec - mt1.tv_sec) + ((double)(mt2.tv_nsec - mt1.tv_nsec)) / 1000000000;
            printf ("Time: %.9lf\n", time);

        }

        free (tokens);
        tokens = NULL;
        tokensCount = 0;
    }

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
