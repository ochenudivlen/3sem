#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>

/*
Программа вычисляет произведение двух больших (2000x2000) матриц с помощью N потоков. Размеры матрицы кратны N. Компилировалась с ключом -О2.
N = 1,   t1  = 34.823321020,  t1/t1 = 1;
N = 2,   t2  = 17.119604383,  t1/t2 = 2.034119;
N = 4,   t3  = 17.766271327,  t1/t3 = 1.960080;
N = 5,   t4  = 17.896341236,  t1/t4 = 1.945835;
N = 10,  t5  = 17.991528981,  t1/t5 = 1.935540;
N = 20,  t6  = 18.018051745,  t1/t6 = 1.932691;
N = 40,  t7  = 18.060753455,  t1/t7 = 1.928121;
N = 50,  t8  = 18.096576095,  t1/t8 = 1.924304;
N = 100, t9  = 18.022376078,  t1/t9 = 1.932227;
N = 200, t10 = 17.845041232, t1/t10 = 1.951428;
*/

const int size = 2000;

struct matrices
{
    int** matrix1;
    int** matrix2;
    int** result_matrix;
    int number;
    int value;
};

void *Matrix_Multiplication (void* arg);

int main()
{
    int N = 0;
    printf ("Size of matrices: %dx%d, N: ", size, size);
    scanf ("%d", &N);

    int** matrix1 = (int**)calloc(size, sizeof (int*));
    for (int i = 0; i < size; i++)
        matrix1[i] = (int*)calloc(size, sizeof (int));

    int** matrix2 = (int**)calloc(size, sizeof (int*));
    for (int i = 0; i < size; i++)
        matrix2[i] = (int*)calloc(size, sizeof (int));

    int** result_matrix = (int**)calloc(size, sizeof (int*));
    for (int i = 0; i < size; i++)
        result_matrix[i] = (int*)calloc(size, sizeof (int));

    srand (time (NULL));

    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            matrix1[i][j] = rand() % 10;

    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            matrix2[i][j] = rand() % 10;

    struct matrices* args = (struct matrices*)calloc(N, sizeof (struct matrices));

    pthread_t* thids = (pthread_t*)calloc(N, sizeof (pthread_t));   //Массив идентификаторов нитей
    int count = 0;

    for (int i = 0; i < N; i++)
    {
        args[i].matrix1 = matrix1;
        args[i].matrix2 = matrix2;
        args[i].result_matrix = result_matrix;
        args[i].value = (size / N);
    }

    struct timespec mt1, mt2;
    double time = 0;

    clock_gettime (CLOCK_REALTIME, &mt1);

    for (int i = 0; i < size; i = i + (size / N))
    {
        args[count].number = i;

        int result = pthread_create (&thids[count], (pthread_attr_t*)NULL, Matrix_Multiplication, &args[count]);

        if (result != 0)
        {
            printf ("Error on thread create, return value = %d\n", result);
            exit (-1);
        }

        count++;
    }

    for (int i = 0; i < N; i++)
        pthread_join (thids[i], (void**)NULL);

    clock_gettime (CLOCK_REALTIME, &mt2);
    time = (mt2.tv_sec - mt1.tv_sec) + ((double)(mt2.tv_nsec - mt1.tv_nsec)) / 1000000000;
    printf ("Time: %.9f\n", time);

    for (int i = 0; i < size; i++)
        free (matrix1[i]);
    free (matrix1);

    for (int i = 0; i < size; i++)
        free (matrix2[i]);
    free (matrix2);

    for (int i = 0; i < size; i++)
        free (result_matrix[i]);
    free (result_matrix);

    free (thids);

    free (args);

    return 0;
}

void *Matrix_Multiplication (void* arg)
{
    struct matrices dummy = *((struct matrices*)arg);

    int strings = dummy.number;
    int columns = 0;

    for (int i = dummy.number; i < dummy.number + dummy.value; i++)
    {
        for (int k = 0; k < size; k++)
        {
            dummy.result_matrix[strings][columns] = 0;
            for (int j = 0; j < size; j++)
                dummy.result_matrix[strings][columns] += (dummy.matrix1[i][j] * dummy.matrix2[j][k]);
            columns++;
        }
    columns = 0;
    strings++;
    }

    return NULL;
}
