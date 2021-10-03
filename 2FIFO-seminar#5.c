#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int const number_of_symbols = 255;

void CreatingFifo (char fifo1[], char fifo2[]);

int main ()
{
    int number = 0;

    printf ("Enter the client number, 1 or 2: ");
    scanf ("%d", &number);

    if (number == 1)
        CreatingFifo ("ab.fifo", "ba.fifo");

    else

    if (number == 2)
        CreatingFifo ("ba.fifo", "ab.fifo");

    else
        printf ("Incorrect data entry\n"); 

    return 0;
}

void CreatingFifo (char fifo1[], char fifo2[])
{
    int fd           = 0, 
        result       = 0;
    size_t size      = 0;
    char* resstring1 = (char*)calloc(number_of_symbols, sizeof (char));
    char* resstring2 = (char*)calloc(number_of_symbols, sizeof (char));

    if (mknod (fifo1, S_IFIFO | 0666, 0) < 0)
    {
//       printf ("Can\'t creat FIFO\n");
//       exit (-1);
   }

    if (mknod (fifo2, S_IFIFO | 0666, 0) < 0)
   {
//        printf ("Can\'t creat FIFO\n");
//        exit (-1);
   }

    if ((result = fork()) < 0)
    {
//        printf("Can\'t open FIFO for writting\n");
//        exit(-1); 
    }

    else

    if (result > 0)
    {
        if ((fd = open (fifo1, O_WRONLY)) < 0)
        {
            printf ("Can\'t open FIFO for writting\n");
            exit(-1); 
        }

    while (1)
    {
        fgets (resstring1, number_of_symbols, stdin);
    
        size = write (fd, resstring1, strlen (resstring1) + 1);
    }
        close (fd);
    }

    else

    {
        if ((fd = open (fifo2, O_RDONLY)) < 0)
        {
            printf ("Can\'t open FIFO for writting\n");
            exit(-1); 
        }

    while (1)
    {
        size = read (fd, resstring2, number_of_symbols);

        printf ("%s", resstring2);
    }
        close (fd);
    }

    free (resstring1);
    free (resstring2);
}
