#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

int const number_of_symbols = 255;

struct Fifo12
{
    char fifo1[128];
    char fifo2[128];
};

void *CreatingFifo (void* arg);
void *FifoRecording (void* arg);
void *FifoReading (void* arg);

int main ()
{
    int number = 0;
    pthread_t thid   = 0; 
    struct Fifo12 Arg;

    printf ("Enter the client number, 1 or 2: ");
    scanf ("%d", &number);

    if (number == 1)
    {
        strcpy (Arg.fifo1, "ab.fifo");
        strcpy (Arg.fifo2, "ba.fifo");
    }

    else

    if (number == 2)
    {
        strcpy (Arg.fifo1, "ba.fifo");
        strcpy (Arg.fifo2, "ab.fifo");
    }

    else
        printf ("Incorrect data entry\n"); 

    CreatingFifo (&Arg);

    int result = pthread_create (&thid, (pthread_attr_t *)NULL, FifoRecording, &Arg);

    if (result != 0)
    {
        printf ("Error on thread create, return value = %d\n", result);
        exit (-1);
    }

    FifoReading (&Arg);

    pthread_join (thid, (void**)NULL);

    return 0;
}

void *CreatingFifo (void* arg)
{
    struct Fifo12 Dummy = *((struct Fifo12*)arg);

    if (mknod (Dummy.fifo1, S_IFIFO | 0666, 0) < 0)
    {
//       printf ("Can\'t creat FIFO\n");
//       exit (-1);
   }

    if (mknod (Dummy.fifo2, S_IFIFO | 0666, 0) < 0)
   {
//        printf ("Can\'t creat FIFO\n");
//        exit (-1);
   }

    return NULL;
}

void *FifoRecording (void* arg)
{
    struct Fifo12 Dummy = *((struct Fifo12*)arg);
    char* resstring1 = (char*)calloc(number_of_symbols, sizeof (char));
    int           fd = 0;
    size_t      size = 0;

    if ((fd = open (Dummy.fifo1, O_WRONLY)) < 0)
    {
        printf ("Can\'t open FIFO for writting\n");
        exit (-1); 
    }

    while (1)
    {
        fgets (resstring1, number_of_symbols, stdin);
        size = write (fd, resstring1, strlen (resstring1) + 1);
    }

    close (fd);

    free (resstring1);

    return NULL;
}

void *FifoReading (void* arg)
{
    struct Fifo12 Dummy = *((struct Fifo12*)arg);
    char* resstring2 = (char*)calloc(number_of_symbols, sizeof (char));
    int           fd = 0;
    size_t      size = 0;

    if ((fd = open (Dummy.fifo2, O_RDONLY)) < 0)
    {
        printf ("Can\'t open FIFO for writting\n");
        exit (-1); 
    }

    while (1)
    {
        size = read (fd, resstring2, number_of_symbols);
        printf ("%s", resstring2);
    }

    close (fd);

    free (resstring2);

    return NULL;
}
