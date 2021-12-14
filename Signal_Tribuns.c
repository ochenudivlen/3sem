#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

void my_handler1 (int nsig);
void my_handler2 (int nsig);

int pid = 0;

int main (void)
{
    pid = fork ();

    (void)signal(SIGUSR1, my_handler1);
    (void)signal(SIGUSR2, my_handler2);

    if (pid == 0)
    {
        kill (getppid(), SIGUSR1);
        while (1);
    }
    else
    if (pid > 0)
    {
        while (1);
    }

}

void my_handler1 (int nsig)
{
    printf ("FRKT\n");
    kill (pid, SIGUSR2);
}

void my_handler2 (int nsig)
{
    printf ("Champion!\n");
    kill (getppid(), SIGUSR1);
}
