#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main()
{
    int n = 0;

    printf ("n: ");
    scanf ("%d", &n);

    for (int i = 0; i < n; i++)
    {
        pid_t pid = fork();

        if (pid == -1)
        {
            printf ("Can\'t fork child\n");
            exit (-1);
        }

        else

        if (pid == 0)
        {
            printf ("My child #%d was created: Pid %d\n", i, getpid());
        }

        else

        if (pid > 0)
        {
            int status = 0;
            int pid1   = wait (&status);
            printf ("My child #%d has ended: Pid %d, Status %d\n", i, pid1, (status >> 8) & 255);
            exit (0);   //Завершаем работу родительского процесса: он не должен перейти к следующей итерации цикла и создать новых потомков
        }
    }

    return 0;
}
