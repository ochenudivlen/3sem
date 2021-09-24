#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int const MAX_NUMBER_OF_SYMBOLS = 100;
int const MAX_NUMBER_OF_WORDS   = 100;

void Split (char* string, char* delimiters, char*** tokens, int* tokensCount);

int main()
{
    char*   string       = (char*)calloc(MAX_NUMBER_OF_SYMBOLS, sizeof (char));
    char*   delimiters   = (char*)calloc(MAX_NUMBER_OF_SYMBOLS, sizeof (char));
    char**  tokens       = NULL;
    int     tokensCount  = 0;

    string     = fgets (string, MAX_NUMBER_OF_SYMBOLS, stdin);
    delimiters = fgets (delimiters, MAX_NUMBER_OF_SYMBOLS, stdin);

    Split (string, delimiters, &tokens, &tokensCount);

    for (int i = 0; i < tokensCount; i++)
        printf ("%s\n", tokens [i]);

    free (tokens);
    free (string);
    free (delimiters);

    return 0;
}

void Split (char* string, char* delimiters, char*** tokens, int* tokensCount)
{
    char *str = strtok (string, delimiters);

    if (str == NULL)
    {
        printf ("Can't split string\n");
        exit(0);
    }

    *tokens = (char**)calloc(MAX_NUMBER_OF_WORDS, sizeof (char*));

    while (str != NULL)
    {
        *tokensCount++;
        *tokens [*tokensCount - 1] = str;
        str = strtok (NULL, delimiters);
    }
}
