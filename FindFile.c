#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

void FindFile (char* fileName, char* currentDir, int depth, int* foundFilesCount, char*** results);

int main()
{
    char fileName[] = "Files.c";
    char currentDir[] = "./";
    int depth = 2;
    int foundFilesCount = 0;
    char **results;

    FindFile (fileName, currentDir, depth, &foundFilesCount, &results);

//    printf ("%d\n", foundFilesCount);

//    for (int i = 0; i < foundFilesCount; i++)
//        printf ("%s\n", results[i]);

    printf ("%s\n", results[foundFilesCount - 1]);

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
    
    while (ep = readdir (dp))
    {
        stat ((ep -> d_name), &buf);        //Проигнорировать файлы "." и ".."

        if (strcmp (ep -> d_name, ".") != 0 && strcmp (ep -> d_name, "..") != 0)
        {

            if (S_ISDIR (buf.st_mode))
            {
                length = strlen (currentDir) + strlen ("/") + strlen(ep -> d_name);
                tempCurrentDir = (char*)malloc(length + 1);
                strcat (tempCurrentDir, currentDir);
                strcat (tempCurrentDir, "/");
                strcat (tempCurrentDir, ep -> d_name);

                FindFile (fileName, tempCurrentDir, depth - 1, foundFilesCount, results);
            }

            if (S_ISREG (buf.st_mode))
                if (strcmp (ep -> d_name, fileName) == 0)
                {
                    (*foundFilesCount)++;
                    printf ("%d\n", (*foundFilesCount));
                    (*results) = (char**)realloc((*results), (*foundFilesCount) * sizeof (char*));
                    length = strlen (currentDir);
                    (*results)[*foundFilesCount - 1] = (char*)malloc(length + 1);
                    strcpy ((*results)[(*foundFilesCount) - 1], currentDir);
                }
        }
    }

    closedir (dp);
}
