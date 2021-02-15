#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "messagequeue.h"

void report_and_exit(const char *message)
{
    perror(message);
    exit(-1);
}

int main()
{
    //Queue
    key_t key = ftok(Path, ProjectId);
    if (key < 0)
    {
        report_and_exit("Error to get key.");
    }

    int qid = msgget(key, 0666 | IPC_CREAT);
    if (qid < 0)
    {
        report_and_exit("Error to get id of queue.");
    }

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

    //Read from queue
    for (int i = 0; i < MessageCount; i++)
    {
        queuedMessage msg;
        if (msgrcv(qid, &msg, sizeof(msg), 3, MSG_NOERROR | IPC_NOWAIT) < 0)
        {
            printf("[%lu:P][%02d:%02d:%02d:%03i][Server] (Server received all messages from queue.)\n", pid, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, time_now.tv_usec);
            exit(-1);
        }
        printf("%s\n", msg.payload);
    }

    if (msgctl(qid, IPC_RMID, NULL) < 0)
    {
        report_and_exit("Error to remove queue.");
    }

    return 0;
}