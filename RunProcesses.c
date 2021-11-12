#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>

//Данная программа запускает все программы из данного файла

int const n  = 100;                                                             //Примем 100 за максимальное количество символов в строке
int const n1 = 10;                                                              //Предполагается, что в первой строке находится число с не более чем десятью знаками

void Split (char* string, char* delimiters, char*** tokens, int* tokensCount);  //Данная функция делит строки на слова

int main ()
{
    char str1[n1];                                                              //Строчка, в которую мы прочитаем первую строку из файла
    int fp = open ("file_text", O_RDONLY);
    int num = read (fp, str1, n1);                                              //Читаем строку размером n1 и запоминаем, сколько символов прочитали

    int number_of_strings = atoi (str1);                                        //В первой строке записано количество запускаемых программ

    close (fp);

    char str[n * (number_of_strings + 1)];                                      //Создадим массив, куда прочитаем весь файл
    fp = open ("file_text", O_RDONLY);                                          //Открываем файл заного, чтобы прочитать его сначала
    num = read (fp, str, n * (number_of_strings + 1));                          //Читаем

    int arr[number_of_strings + 1];                                             //Номера символов, означающих переход на новую строку
    int count = 0;                                                              //Считаем количество символов перехода на новую строку

    for (int i = 0; i < num; i++)
        if (str[i] == '\n')
        {
            arr[count] = i;
            count++;
        }

    char** strings = (char**)calloc(number_of_strings, sizeof (char*));         //Выделяем память для строк, в которых содержаться данные о запускаемых программах

    for (int i = 0; i < number_of_strings; i++)                                 //Разбиение на строки
    {
        strings[i] = (char*)calloc(arr[i + 1] - arr[i], sizeof (char));         //Длина одной строки
        int count = 0;
        for (int j = arr[i] + 1; j < arr[i + 1]; j++)                           //Посимвольно копируем строчки
        {
            strings[i][count] = str[j];
            count++;
        }
    }

    char*   delimiters  = " ";                                                  //Раазделитель
    char**  tokens      = NULL;                                                 //Двойной массив, содержащий строки, поделённые на слова
    int     tokensCount = 0;                                                    //Количество слов
    int* time = (int*)calloc(number_of_strings, sizeof (int));                  //Выделение памяти для массива времен
    int cpids[number_of_strings];                                               //Массив ID дочерних процессов
    int statusCpids[number_of_strings];                                         //Массив статусов, с которыми завершились процессы

    for (int i = 0; i < number_of_strings; i++)                                 //Создадим массив времён
    {
        char* temp_str;                                                         //Временная переменная, где хранится строка
        strcpy (temp_str, strings[i]);                                          //копируем в неё строку

        Split (strings[i], delimiters, &tokens, &tokensCount);                  //Делим строку на слова
        time[i] = atoi (tokens[tokensCount - 1]);   //Преобразуем строку в число

        strcpy (strings[i], temp_str);                                          //Возвращаем в элемент массива его содержимое (сплит разрушает строчку, поэтому пришлось создавать временное хранилище

        free (tokens);                                                          //Освобождаем память
        tokens = NULL;
        tokensCount = 0;
    }

    for (int i = 0; i < (number_of_strings - 1); i++)                           //Отсортируем команды по времени
        for (int j = 1; j < (number_of_strings - i); j++)
            if (time[j] < time[j - 1])
            {
                char* temp_str = strings[j];
                strings[j] = strings[j - 1];
                strings[j - 1] = temp_str;

                int temp_time = time[j];
                time[j] = time[j - 1];
                time[j - 1] = temp_time;
            }

    for (int i = 0; i < number_of_strings; i++)                                 //Выполнение команд
    {
        Split (strings[i], delimiters, &tokens, &tokensCount);                  //Опять делим строки на слова

        tokens[tokensCount - 1] = NULL;                                         //Последнему слову в каждой строчке присваиваем нулевой указатель

        int first_pipe[2] = {0};                                                //Массив для первого pipe

        if (pipe (first_pipe) < 0)                                              //Создаем pipe
        {
            printf ("Can't create pipe\n");
            exit (-1);
        }

        pid_t pid1 = fork();                                                    //Создаем дочерний процесс

        if (pid1 < 0)                                                           //Если не получилось - сообщаем об этом
        {
            printf ("Can't fork child\n");
            exit (-1);
        }

        else if (pid1 == 0)                                                     //Если это дочерний процесс
        {
            close (first_pipe[0]);                                              //Закрываем входной поток

            int cpid = getpid();                                                //Идентификатор дочернего процесса
            int size = write (first_pipe[1], &cpid, sizeof (cpid));             //Записываем в pipe идентификатор дочернего процесса
            if (size != sizeof (cpid))                                          //Если не удалось - сообщаем об этом
            {
                printf ("Can't write all cpid\n");
                exit (-1);
            }

            close (first_pipe[1]);                                              //Закрываем выходной поток

            if (i == 0)                                                         //Если это первый проход цикла - просто ставим задержку времени
                sleep (time[i]);

            else                                                                //Если не первый проход - ставим задержку времени, равную разности времён
                sleep (time[i] - time[i - 1]);

            execvp (tokens[0], tokens);                                         //Запускаю программу, названия и параметры которой прочитали из файла

            printf ("exit pid1\n");                                             //На случай, если что-то пошло не по плану
            exit (0);
        }

        else                                                                    //Если это родительский процесс
        {
            close (first_pipe[1]);                                              //Закрываем выходной поток

            int size = read (first_pipe[0], &cpids[i], sizeof (int));           //Читаем из pipe'а идентификатор дочернего процесса
            if (size < sizeof (int))                                            //Если не получилоь - сообщаем об этом
            {
                printf ("Can't read cpid\n");
                exit (-1);
            }

            close (first_pipe[0]);                                              //Закрываем входной поток

            pid_t pid2 = fork();                                                //Создаем ещё один дочерний процесс, который будет отслеживать, уложилась ли в timeout считанная из файла программа

            if (pid2 < 0)                                                       //Если не получилось создать - сообщаем об этом
            {
                printf ("Can't fork child\n");
                exit (-1);
            }

            else if (pid2 == 0)                                                 //Если это дочерний процесс
            {
                if (i == 0)                                                     //Если первый проход цикла - ставим обычную задержку времени плюс 5 секунд, отведенные на timeout
                    sleep (time[i] + 5);

                else
                    sleep (time[i] - time[i - 1] + 5);                          //Если проход не первый, ставим задержку на разность времен плюс timeout

                kill (cpids[i], SIGTERM);                                       //Убиваем процесс в не зависимости от того, завершился он или нет, плюс запомниаем статусы, с которыми завершились процессы, выполнявшие программмы (здесь использовали идентификатор предыдущего дочернего процесса, дабы мы смогли убить его)

                exit(0);                                                        //Выходим из этого процесса
            }

            waitpid (cpids[i], &statusCpids[i], 0);                             //Находим статусы, с которыми заврешились процессы, выполнявшие программы из файла

            free (tokens);                                                      //Очищаем память и обнуляем переменные, которыми будем пользоваться при следующем проходе
            tokens = NULL;
            tokensCount = 0;
        }
    }

    for (int i = 0; i < number_of_strings; i++)                                 //Вывод статусов, с которыми завершились процессы
    {
        printf ("N: %d, Pid: %d, Status: %d, ", i, cpids[i], statusCpids[i]);

        if (statusCpids[i] == 15)                                               //Было замечено, что если статус 15, то это из-за kill'a
            printf ("Stopped with kill\n");

        else                                                                    //Иначе причина в чем другом (вероятно, процесс завершился сам)
            printf ("Ended on its own or for another reason\n");
    }

    free (time);                                                                //Очищаем память

    for (int i = 0; i < number_of_strings; i++)
        free (strings[i]);

    free (strings);

    return 0;
}

void Split (char* string, char* delimiters, char*** tokens, int* tokensCount)   //Функция, делящая строчку на слова
{
    char *str = strtok (string, delimiters);                                    //Пробуем достать первое слово

    if (str == NULL)                                                            //Если не получилось - сообщаем о том, что строку разбить невозможно
    {
        printf ("Can't split string\n");
        return;
    }

    while (str != NULL)                                                         //Последовательно вызываем strtok для деления строки на слова
    {
        (*tokensCount)++;
        *tokens = (char**)realloc(*tokens, *tokensCount * sizeof (char*));      //Добавляем в массив элемент для нового слова
        (*tokens) [*tokensCount - 1] = str;
        str = strtok (NULL, delimiters);
    }
}

/*Пример содержимого считываемого файла
3
ls -al 6
pwd 3
echo Hello, world! 9
*/
