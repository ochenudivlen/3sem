#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <stdlib.h>

//Эта программа проводит поиск файла с определенным именем, ДЗ для семинара 20.11.2021

void FindFile (char* fileName, char* currentDir, int depth, int* foundFilesCount, char*** results);

int main (int argc, char **argv)
{
    char* currentDir = argv[1];
    int depth = atoi (argv[2]); 
    char* fileName = argv[3];
    int foundFilesCount = 0;
    char **results;

    FindFile (fileName, currentDir, depth, &foundFilesCount, &results);

    for (int i = 0; i < foundFilesCount; i++)
    {
        printf ("%s\n", results[i]);
        free (results[i]);
    }

    if (foundFilesCount == 0)
        printf ("File not found\n");

    free (results);

    return 0;
}

void FindFile (char* fileName, char* currentDir, int depth, int* foundFilesCount, char*** results)
{
    if (depth < 1)
        return;

    DIR *dp;
    struct dirent *ep;
    char *tempCurrentDir = NULL;
    int length = 0;

    dp = opendir (currentDir);

    struct stat buf;
    
    while ((ep = readdir (dp)) != NULL)
    {
        stat ((ep -> d_name), &buf);

        if (strcmp (ep -> d_name, ".") != 0 && strcmp (ep -> d_name, "..") != 0)
        {
            length = strlen (currentDir) + strlen ("/") + strlen(ep -> d_name);
            tempCurrentDir = (char*)calloc(length + 1, sizeof (char));
            strcat (tempCurrentDir, currentDir);
            strcat (tempCurrentDir, "/");
            strcat (tempCurrentDir, ep -> d_name);

            if (ep -> d_type == DT_DIR)
            {
                FindFile (fileName, tempCurrentDir, depth - 1, foundFilesCount, results);
                free (tempCurrentDir);
            }

            else

            if (strcmp (ep -> d_name, fileName) == 0)
            {
                (*foundFilesCount)++;

                if ((*foundFilesCount) == 1)
                    (*results) = (char**)calloc((*foundFilesCount), sizeof (char*));
                else
                    (*results) = (char**)realloc((*results), (*foundFilesCount) * sizeof (char*));

                length = strlen (tempCurrentDir);
                (*results)[*foundFilesCount - 1] = (char*)malloc(length + 1);
                strcpy ((*results)[(*foundFilesCount) - 1], tempCurrentDir);
            }
        }
    }

    closedir (dp);
}
