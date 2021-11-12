#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

int main ()
{
    DIR *dp;
    struct dirent *ep;

    dp = opendir ("./");

    struct stat buf;
    
    if (dp != NULL)
    {
        while (ep = readdir (dp))
        {
            printf ("Name of file: %35s, ", ep -> d_name);

            stat ((ep -> d_name), &buf);

            printf ("size of file: %10ld\n", buf.st_size);
        }
        
        closedir (dp);
    }

    else
        printf ("Couldn't open the directory\n");

    return 0;
}
