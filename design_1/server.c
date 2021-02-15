#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <string.h>

#define FileName "design1.dat"

void report_and_exit(const char *message)
{
    perror(message);
    exit(-1);
}

void consume()
{
    struct flock key;
    key.l_type = F_WRLCK;
    key.l_whence = SEEK_SET;
    key.l_start = 0;
    key.l_len = 0;
    key.l_pid = getpid();

    
    int fileDesc;
    if ((fileDesc = open(FileName, O_RDONLY)) < 0)
        report_and_exit("File could not be opened...");

    fcntl(fileDesc, F_GETLK, &key);
    if (key.l_type != F_UNLCK)
        report_and_exit("File is locked...");

    key.l_type = F_RDLCK;
    if (fcntl(fileDesc, F_SETLK, &key) < 0)
        report_and_exit("can't write to file while reading...");

    int buf;
    
    while (read(fileDesc, &buf, 1) > 0)
    {
        write(STDOUT_FILENO, &buf, 1);
    }
    
    key.l_type = F_UNLCK;
    if (fcntl(fileDesc, F_SETLK, &key) < 0)
        report_and_exit("unlocking is failed...");

    close(fileDesc);
    return 0;
}

void producer(char *message)
{
    //printf("%s\n", message);
    struct flock key;
    key.l_type = F_WRLCK;
    key.l_whence = SEEK_SET;
    key.l_start = 0;
    key.l_len = 0;
    key.l_pid = getpid(); // id of process

    int fileDesc;
    if ((fileDesc = open(FileName, O_RDWR | O_CREAT, 0666)) < 0)
    {
        //printf("fist if\n");
        report_and_exit("File could not be opened...");
    }

    if (fcntl(fileDesc, F_SETLK, &key) < 0)
    {
        //printf("second if\n");
        report_and_exit("fcntl failed to get key...");
    }
    else
    {
        //printf("else1\n");
        write(fileDesc, message, strlen(message)); 
        //printf("else2\n");
    }

    key.l_type = F_UNLCK;
    if (fcntl(fileDesc, F_SETLK, &key) < 0)
        report_and_exit("Unlocking process is failed.");

    close(fileDesc);
    return 0;
}

int main()
{
    //to choose read or write
    int option;
    char message[500];
    char enteredMessage[500];

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
    printf("To read the file press 1; To write the file press 2; To exit the server press 3\n");
    scanf("%d", &option);

    while (option < 3)
    {
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

        if (option == 1)
        {
            //read message
            printf(" [%lu:P][%02d:%02d:%02d:%03i][Server] (Server read message from file", pid, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, time_now.tv_usec);
            consume();
            printf(")\n");
        }

        if (option == 2)
        {
            //Enter server note:
            printf("[%lu:P][%02d:%02d:%02d:%03i][Server] Please write your message:\n", pid, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, time_now.tv_usec);
            scanf(" %[^\t\n]s", &enteredMessage);
            //scanf("%s", enteredMessage);
            strcpy(message, enteredMessage);
            //Show entered message
            printf("[%lu:P][%02d:%02d:%02d:%03i][Server] (Server tries to writes message: “%s”)\n", pid, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, time_now.tv_usec, message);

            // write message
            producer(message);
        }

        printf("To read the file press 1; To write the file press 2; To exit the server press 3\n");
        scanf("%d", &option);
    }

    return 0;
}