#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void GenerateString (int n, char* string);

int main()
{
    int number;
    scanf ("%d", &number);

    char* s;

    GenerateString (number, s);

    return 0;
}

void GenerateString (int n, char* string)
{
    int   val = 1; 
    char  more_string;
    char* temp_string;
    temp_string = (char*)calloc(1, sizeof (char));

    more_string = 'a'; 

    printf ("%c\n", more_string);

    temp_string[0] = more_string;
    more_string = more_string + 1;
    val = (val * 2) + 1;
 
    for (int i = 1; i < n; i++)
    {
        string = (char*)calloc(val, sizeof (char));

        strcpy (string, temp_string);
        string [strlen(string)] = more_string;
        strcat (string, temp_string);

        more_string = more_string + 1;
        temp_string = string;
        val = (val * 2) + 1;

        printf ("%s\n", string);
    }

    free (string);
}

// TODO: вы позвали calloc n+1 раз, а free только 1 раз
