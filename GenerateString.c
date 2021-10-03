#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void GenerateString (int n, char* string);

int main()
{
    int number;
    scanf ("%d", &number);

    char* s = NULL;

    GenerateString (number, s);

    return 0;
}

void GenerateString (int n, char* string)
{
    int  val = 1; 
    char more_string = 'a';

    printf ("%c\n", more_string);

    for (int i = 0; i < (n - 1); i++)
        val = (val * 2) + 1;

    string = (char*)calloc((val * 2) + 1, sizeof (char));
    char* temp_string = (char*)calloc(val, sizeof (char));
 
    temp_string[0] = more_string;
    more_string++;

    for (int i = 1; i < n; i++)
    {
        strcpy (string, temp_string);
        string [strlen(string)] = more_string;
        strcat (string, temp_string);

        strcpy (temp_string, string);
        more_string++;

        printf ("%s\n", string);
    }

    free (string);
    free (temp_string);
}
