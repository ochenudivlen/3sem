#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

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

    Split (strings[0], delimiters, &tokens, &tokensCount);

    char* str_temp = tokens[0];

    for (int i = 0; i < tokensCount - 1; i++)
        tokens[i] = tokens[i + 1];

    tokens [tokensCount - 1] = NULL;

    execvp (str_temp, tokens);

//    execlp (tokens[0], tokens[0], tokens[1], NULL);

//    printf ("%s-\n", tokens[0]);
//    printf ("%d\n", number_of_string);
//    while (num != 0)
//    {
//       write (1, str, num);
//        printf ("\n");
//        num = read (fp, str, N);
//    }
//    execlp ("ls", "ls", "-al", NULL);

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
