#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include "sharedmem.h"
#include <time.h>

void report_and_exit(const char *message)
{
    perror(message);
    exit(-1);
}

void memoryReader()
{
    int fileDesc = shm_open(BackingFile, O_RDWR, AccessPerms);
    if (fileDesc < 0)
        report_and_exit("Error with file descriptor.");

    caddr_t memorypointer = mmap(NULL, ByteSize, PROT_READ | PROT_WRITE, MAP_SHARED, fileDesc, 0);

    if ((caddr_t)-1 == memorypointer)
        report_and_exit("Error with accessing memory segment.");

    sem_t *semaphoreRead = sem_open(SemaphoreRead, O_CREAT, AccessPerms, 0);

    if (semaphoreRead == (void *)-1)
        report_and_exit("semaphore open");

    //waits on a semaphore (read_semaphore which is initially 0)
    if (!sem_wait(semaphoreRead))
    {
        int i;
        for (i = 0; i < strlen(MemContents); i++)
            write(STDOUT_FILENO, memorypointer + i, 1);
        sem_post(semaphoreRead);
    }

    munmap(memorypointer, ByteSize);
    close(fileDesc);
    sem_close(semaphoreRead);
    unlink(BackingFile);
    return 0;
}

int main()
{
    // get process id
    pid_t pid = getpid();
    //printf("pid: %lu\n", pid);

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
    printf("[%lu:C][%02d:%02d:%02d:%03i][Server] (Server has started.)\n", pid, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, time_now.tv_usec);

    //Read message from memory
    printf(" [%lu:C][%02d:%02d:%02d:%03i][Server] (Server read message from memory)\n", pid, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, time_now.tv_usec);
    memoryReader();

    return 0;
}