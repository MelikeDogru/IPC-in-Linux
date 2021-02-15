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

#define AccessPerms 0666
#define AccessPermsSemp 0644

int main()
{

  char name[50];
  char message[800];
  char enteredName[50];
  char enteredMessage[800];

  const char *pipeName = "./pipeName";
  mkfifo(pipeName, AccessPerms);

  sem_t *semaphoreRead = sem_open("readSemaphore", O_CREAT, AccessPermsSemp, 0);
  sem_t *semaphoreWrite = sem_open("writeSemaphore", O_CREAT, AccessPermsSemp, 1);

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
  //Enter client name:
  printf("[%lu:C][%02d:%02d:%02d:%03i][] Please enter your name:\n", pid, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, time_now.tv_usec);
  scanf("%s", enteredName);
  strcpy(name, enteredName);
  //Show entered name
  printf("[%lu:C][%02d:%02d:%02d:%03i][%s] (Client name is set to “%s”)\n", pid, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, time_now.tv_usec, name, name);

  //Enter client note:
  printf("[%lu:C][%02d:%02d:%02d:%03i][%s] Please write your message:\n", pid, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, time_now.tv_usec, name);
  scanf(" %[^\t\n]s", &enteredMessage);
  strcpy(message, enteredMessage);

  //Show entered message
  printf("[%lu:C][%02d:%02d:%02d:%03i][%s] (Tring to write message: “%s”)\n", pid, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, time_now.tv_usec, name, message);

  // write message
  //open pipe for writing
  int fileDesc = open(pipeName, O_CREAT | O_WRONLY);

  if (fileDesc < 0)
  {
    printf("Pipe could not be opened to wite\n");
    return -1;
  }

  //sem_wait(semaphoreWrite);

  //write message to pipe
  if (!sem_wait(semaphoreWrite))
  {
    write(fileDesc, message, strlen(message));
    sem_post(semaphoreWrite);
  }

  sem_post(semaphoreWrite);
  sem_post(semaphoreRead);

  sem_close(semaphoreRead);
  sem_close(semaphoreWrite);

  //close file decriptor and unlink pipe/file
  close(fileDesc);
  unlink(pipeName);

  return 0;
}
