#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <semaphore.h>

int main()
{
    // get process id
    pid_t pid = getpid();

    // get current time
    time_t rawtime = time(NULL);
    if (rawtime == -1)
    {
        puts("Error to get time");
        return 1;
    }
    struct tm *ptm = localtime(&rawtime);
    if (ptm == NULL)
    {
        puts("Error to get time");
        return 1;
    }
    struct timeval time_now;
    gettimeofday(&time_now, NULL);

    //Start Server
    printf("[%lu:P][%02d:%02d:%02d:%03i][Server] (Server has started.)\n", pid, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, time_now.tv_usec);

    //Read message
    sem_t *semaphoreRead = sem_open("readSemaphore", O_CREAT, S_IRWXU | S_IRWXO | S_IRWXG, 0);
    sem_t *semaphoreWrite = sem_open("writeSemaphore", O_CREAT, S_IRWXU | S_IRWXO | S_IRWXG, 1);
    //open file to read
    const char *fileName = "./pipeName";
    int fileDesc = open(fileName, O_RDONLY);

    if (fileDesc < 0)
    {
        printf("Pipe could not be opened to read\n");
        return -1;
    }

    //read message
    sem_wait(semaphoreRead);
    int buffer;
    while (read(fileDesc, &buffer, 1) > 0)
        write(STDOUT_FILENO, &buffer, sizeof(buffer));

    printf("\n");
    // close pipe and unlink
    sem_close(semaphoreRead);
    sem_close(semaphoreWrite);
    close(fileDesc);
    unlink(fileName);

    return 0;
}