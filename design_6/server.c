#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include "socket.h"
#include <time.h>

void report_and_terminate(const char *message, int terminate)
{
    perror(message);
    if (terminate)
    {
        exit(-1);
    }
}

int main()
{
    //Socket
    int fileDesc = socket(AF_INET, SOCK_STREAM, 0);
    if (fileDesc < 0)
    {
        report_and_terminate("socket", 1);
    }

    //Connecting addresses
    struct sockaddr_in saddress;
    memset(&saddress, 0, sizeof(saddress));
    saddress.sin_family = AF_INET;
    saddress.sin_addr.s_addr = htonl(INADDR_ANY);
    saddress.sin_port = htons(PortNumber);

    if (bind(fileDesc, (struct sockaddr *)&saddress, sizeof(saddress)) < 0)
    {
        report_and_terminate("bind", 1);
    }

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
    printf("[%lu:P][%02d:%02d:%02d:%03i][Server] (Server has started.)\n", pid, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, time_now.tv_usec);

    //Read

    if (listen(fileDesc, MaxConnects) < 0)
    {
        report_and_terminate("listen", 1);
    }
    fprintf(stderr, "Server 's listening port with number of %i for clients...\n", PortNumber);

    while (1)
    {
        struct sockaddr_in clientaddress;
        int lenght = sizeof(clientaddress);

        int cfileDesc = accept(fileDesc, (struct sockaddr *)&clientaddress, &lenght);
        if (cfileDesc < 0)
        {
            report_and_terminate("accept", 0);
            continue;
        }

        for (int i = 0; i < MessageLen; i++)
        {
            char buff[BufferSize + 1];
            memset(buff, '\0', sizeof(buff));
            int number = read(cfileDesc, buff, sizeof(buff));
            if (number > 0)
            {
                puts(buff);
                write(cfileDesc, buff, sizeof(buff));
            }
        }

        close(cfileDesc);
    }

    return 0;
}