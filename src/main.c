#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

// Task: make interpreter for command line processing -> ls -al | grep "May" | wc -l > a.txt

inline void ErrorHandler(const char* arg)
{
    perror(arg);
    exit(EXIT_FAILURE);
}

int main ()
{
    int fd1[2], fd2[2], status;
    pid_t ChildProcess;

    FILE* fout = fopen("output.txt", "w");

    if (pipe(fd1), pipe(fd2) == -1) ErrorHandler("pipe()");

    // Первое порождение
    if ((ChildProcess = fork()) == -1) ErrorHandler("fork()");

    if (ChildProcess == 0)
    {
        fprintf(fout, "1: Был создан!\n");

        close(fd2[0]); // Закрываем
        close(fd2[1]); // ненужные
        close(fd1[0]); // дескрипторы

        close(1);      // Закрываем stdout, в порожденном процессе
        dup(fd1[1]);   // Дублируем дескриптор для записи (будет наименьший возможный, то есть 1. Таким образом, заменяем stdout на fd1[1])
        close(fd1[1]); // Закрываем оригинальный дескриптор

        fprintf(fout ,"1: Начата работа \"ls -al\"\n");
        fclose(fout);

        execl("/bin/ls", "ls", "-al", NULL);

        ErrorHandler("execl()");
    }
    
    sleep(1);

    // Второе порождение
    if ((ChildProcess = fork()) == -1) ErrorHandler("fork()");

    if (ChildProcess == 0)
    {
        fprintf(fout, "2: Был создан!\n");

        close(fd1[1]); // Закрываем
        close(fd2[0]); // ненужные дескрипторы

        close(0);      // Закрываем stdin
        dup(fd1[0]);   // заменяя его dup(fd1[0])
        close(fd1[0]); // и закрываем оригинальный дескриптор
        
        close(1);      // Закрываем stdout
        dup(fd2[1]);   // заменяя его dup(fd2[1])
        close(fd2[1]); // и закрываем оригинальный дескриптор

        fprintf(fout, "2: Начата работа \"grep May\"!\n");
        fclose(fout);

        execl("/usr/bin/grep", "grep", "May", NULL);

        ErrorHandler("execl()");
    }

    sleep(1);

    // Третье порождение
    if ((ChildProcess = fork()) == -1) ErrorHandler("fork()");

    if (ChildProcess == 0)
    {
        fprintf(fout, "3: Был создан!\n");

        close(fd1[0]); // Закрываем
        close(fd1[1]); // ненужные
        close(fd2[1]); // дескрипторы

        close(0); // Закрываем stdin
        dup(fd2[0]); // Заменяя его на dup(fd[0])

        fprintf(fout ,"3: Начата работа \"wc -l > a.txt\"\n");
        fclose(fout);

        execl("/bin/sh", "sh", "-c", "wc -l > a.txt", NULL);

        ErrorHandler("execl()");
    }

    sleep(1);

    wait(&status);
    fprintf(fout, "1: \"ls -al\" завершил свою работу\n");
    
    fprintf(fout, "2: \"grep May\" завершил свою работу\n");
 
    fprintf(fout, "3: \"wc -l > a.txt\" завершил свою работу\n");
    fclose(fout);

    return EXIT_SUCCESS;
}