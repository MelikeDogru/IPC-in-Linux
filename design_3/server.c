#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <time.h>
#include <semaphore.h>
#include <fcntl.h>

#define ReadEnd 0
#define WriteEnd 1
#define SemaphoreRead "readsemaphore"
#define SemaphoreWrite "writesemaphore"
#define AccessPerms 0644

int pipeFDs[2];
char buffer;


void report_and_exit(const char *messageString)
{
    perror(messageString);
    exit(-1); 
}
int main()
{

    pid_t wpid;
    int status = 0;
    
    sem_t *semaphoreRead = sem_open(SemaphoreWrite, O_CREAT, AccessPerms, 0);

    char name[50];
    char message[500];
    char enteredName[50];
    char enteredMessage[500];

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
    printf("[%lu:P][%02d:%02d:%02d:%03i][Server] (Server/Parent Process has started.)\n", getpid(), ptm->tm_hour, ptm->tm_min, ptm->tm_sec, time_now.tv_usec);

    //Create 1 child processes
    for (int i = 0; i < 1; i++)
    {
        if (fork() == 0)
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

            //Server created children process output
            printf("[%lu:P][%02d:%02d:%02d:%03i][Server] (Server/Parent Process created first client with PID: %d.)\n", getppid(), ptm->tm_hour, ptm->tm_min, ptm->tm_sec, time_now.tv_usec, getpid());

            //Start Client
            printf("[%lu:C][%02d:%02d:%02d:%03i][] (Client has been started.)\n", getpid(), ptm->tm_hour, ptm->tm_min, ptm->tm_sec, time_now.tv_usec);

            //Enter client name:
            printf("[%lu:C][%02d:%02d:%02d:%03i][] Please enter your name:\n", getpid(), ptm->tm_hour, ptm->tm_min, ptm->tm_sec, time_now.tv_usec);
            scanf("%s", enteredName);
            strcpy(name, enteredName);
            //Show entered name
            printf("[%lu:C][%02d:%02d:%02d:%03i][%s] (Client name is set to “%s”)\n", getpid(), ptm->tm_hour, ptm->tm_min, ptm->tm_sec, time_now.tv_usec, name, name);

            //Enter client note:
            printf("[%lu:C][%02d:%02d:%02d:%03i][%s] Please write your message:\n", getpid(), ptm->tm_hour, ptm->tm_min, ptm->tm_sec, time_now.tv_usec, name);
            scanf(" %[^\t\n]s", &enteredMessage);
            //scanf("%s", enteredMessage);
            strcpy(message, enteredMessage);

            //Show entered message
            printf("[%lu:C][%02d:%02d:%02d:%03i][%s] (Tring to write message: “%s”)\n", getpid(), ptm->tm_hour, ptm->tm_min, ptm->tm_sec, time_now.tv_usec, name, message);

            
            // initialize write semaphore wit value of 1
            sem_t *semaphoreWrite = sem_open(SemaphoreWrite, O_CREAT, AccessPerms, 0);
            if (semaphoreWrite == (void *)-1)
            {
                report_and_exit("semaphore open");
            }
           
            // close file descriptor to not be read
            close(pipeFDs[ReadEnd]);
            // ope file descriptor to write
            open(pipeFDs[WriteEnd], O_RDWR);
            
            // check semaphore for writing message
            sem_wait(semaphoreWrite);
           // {
                //printf("selam4\n");
                write(pipeFDs[WriteEnd], message, strlen(message));
           // }

            // increment the read semaphore 
            if (sem_post(semaphoreRead) < 0)
                report_and_exit("semaphore post");

            //  sleep(12); 

            //printf("selam5\n");
            // close file descriptor to not be writen and write semaphore
            sem_close(semaphoreWrite);
            close(pipeFDs[WriteEnd]);

            //wait(NULL);

            exit(0);
        }

        else
        { //Parent
            
            while ((wpid = wait(&status)) > 0);

            //read message
            printf("[%lu:P][%02d:%02d:%02d:%03i][Server] (Server read message from file)\n", getpid(), ptm->tm_hour, ptm->tm_min, ptm->tm_sec, time_now.tv_usec);

            printf("reading");

            if (pipe(pipeFDs) < 0)
                report_and_exit("pipeFD");

            // close file descriptor to not be writen 
            close(pipeFDs[WriteEnd]);
            // check read semaphore
            if (!sem_wait(semaphoreRead))
            {
                while (read(pipeFDs[ReadEnd], &buffer, 1) > 0)  
                    write(STDOUT_FILENO, &buffer, sizeof(buffer)); 
            }
            // close file descriptor to not be read
            close(pipeFDs[ReadEnd]);

            for (int i = 0; i < 1; i++)
            {
                wait(NULL);
            }
        }
    }

    //Parent Process

    return 0;
}
