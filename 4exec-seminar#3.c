#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main ()
{
    int pid = fork();

    if (pid < 0)
    {
        printf ("Can\'t fork child\n");
        exit (-1);
    }

    else

    if (pid == 0)
    {
        execlp ("gcc", "gcc", "HelloWorld.c", "-o", "myprog", NULL);
    }

    else

    if (pid > 0)
    {
        execlp ("./myprog", "./myprog", NULL);
    }

    printf ("Error\n");

    return 0;
}
