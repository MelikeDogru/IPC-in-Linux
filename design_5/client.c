#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include "messagequeue.h"
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>

#define AccessPermsSemp 0644

void report_and_exit(const char *message)
{
  perror(message);
  exit(-1);
}

int main()
{
  int option;

  char name[50];
  char message[500];
  char enteredName[50];
  char enteredMessage[500];

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

  //Start Client
  printf("[%lu:C][%02d:%02d:%02d:%03i][] (Client has been started.)\n", pid, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, time_now.tv_usec);
  printf("To create a new client please press 1. If you want to exit, press 2\n");
  scanf("%d", &option);

  int counter = 0;
  char *payloads[MessageCount];

  sem_t *semaphoreWrite = sem_open("writeSemaphore", O_CREAT, AccessPermsSemp, 1);

  while (option < 2 && counter < MessageCount)
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

    //Enter client name:
    printf("[%lu:C][%02d:%02d:%02d:%03i][] Please enter your name:\n", pid, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, time_now.tv_usec);
    scanf("%s", enteredName);
    strcpy(name, enteredName);
    //Show entered name
    printf("[%lu:C][%02d:%02d:%02d:%03i][%s] (Client name is set to “%s”)\n", pid, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, time_now.tv_usec, name, name);

    //Enter client note:
    printf("[%lu:C][%02d:%02d:%02d:%03i][%s] Please write your message:\n", pid, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, time_now.tv_usec, name);
    scanf(" %[^\t\n]s", &enteredMessage);
    //scanf("%s", enteredMessage);
    strcpy(message, enteredMessage);

    //Show entered message
    printf("[%lu:C][%02d:%02d:%02d:%03i][%s] (Tring to write message: “%s”)\n", pid, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, time_now.tv_usec, name, message);

    // add message to queue
    if (!sem_wait(semaphoreWrite))
    {
      payloads[counter] = message;
      queuedMessage msg;
      msg.type = 3;
      strcpy(msg.payload, payloads[counter]);

      msgsnd(qid, &msg, sizeof(msg), IPC_NOWAIT);
      printf("[%lu:C][%02d:%02d:%02d:%03i][%s] (Message written to message queue: “%s”)\n", pid, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, time_now.tv_usec, name, msg.payload);

      counter = counter + 1;

      sem_post(semaphoreWrite);
    }

    sem_post(semaphoreWrite);

    printf("To create a new client please press 1. If you want to exit and close the queue, press 2\n");
    scanf("%d", &option);
  }

  sem_close(semaphoreWrite);

  return 0;
}
