#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    int first_pipe[2]  = {0};
    size_t size;

    if (pipe (first_pipe) < 0)
    {
        printf ("Can\'t create pipe\n");
        exit (-1);
    }

    int second_pipe[2] = {0};

    if (pipe (second_pipe) < 0)
    {
        printf ("Can\'t create pipe\n");
        exit (-1);
    }

    int result = 0;

    result = fork ();

    int transmitted_number_1 = 2;
    int transmitted_number_2 = 3;

    if (result < 0)
    {
        printf ("Can\'t fork child\n");
        exit (-1);
    }

    else

    if (result > 0) 
    {
        close (first_pipe[0]);

        size = write (first_pipe[1], &transmitted_number_1, sizeof (transmitted_number_1));
    
        if (size != sizeof (transmitted_number_1))
        {
            printf ("Can't write number\n");
            exit (-1);
        }

        size = write (first_pipe[1], &transmitted_number_2, sizeof (transmitted_number_2));
    
        if (size != sizeof (transmitted_number_2))
        {
            printf ("Can't write number\n");
            exit (-1);
        }

        close (first_pipe[1]);

        int result_number = 0;

        size = read (second_pipe[0], &result_number, sizeof (result_number));

        if (size < 0)
        {
            printf ("Can't read number\n");
            exit (-1);
        }

        printf ("%d\n", result_number);

        close (second_pipe[0]);

        printf ("Parent exit\n");
    } 

    else 

    {
        close (first_pipe[1]);

        int received_numbers[2] = {0};

        size = read (first_pipe[0], received_numbers, 2 * sizeof (received_numbers[0]));

        if (size < 0)
        {
            printf("Can't read numbers\n");
            exit(-1);
        }

        int returned_number = received_numbers[0] + received_numbers[1];

        size = write (second_pipe[1], &returned_number, sizeof (returned_number));

        if (size != sizeof (returned_number))
        {
            printf ("Can't write number\n");
            exit (-1);
        }

        close (first_pipe[0]);
        close (second_pipe[1]);

    }

    return 0;
} 
