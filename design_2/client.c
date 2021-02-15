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

void memoryWriter(char *message)
{
  int fileDesc = shm_open(BackingFile, O_RDWR | O_CREAT, AccessPerms);

  if (fileDesc < 0)
    report_and_exit("Shared memory segment could not be opened.");

  ftruncate(fileDesc, ByteSize);

  caddr_t memoryptr = mmap(NULL, ByteSize, PROT_READ | PROT_WRITE, MAP_SHARED, fileDesc, 0);

  if ((caddr_t)-1 == memoryptr)
    report_and_exit("Segment could not be gotten.");


  sem_t *semaphoreWrite = sem_open(SemaphoreWrite, O_CREAT, AccessPerms, 1);
  sem_t *semaphoreRead = sem_open(SemaphoreRead, O_CREAT, AccessPerms, 0);

  if (semaphoreRead == (void *)-1)
  {
    report_and_exit("semaphore open");
  }

  //waits on a write_semaphor (initially 1)
  if (!sem_wait(semaphoreWrite))
  { 
    strcpy(memoryptr, message);
  }

  strcpy(memoryptr, message);

  if (sem_post(semaphoreRead) < 0)
    report_and_exit("semophore increment");

  sleep(12);

  munmap(memoryptr, ByteSize);
  close(fileDesc);
  sem_close(semaphoreRead);
  sem_close(semaphoreWrite);
  shm_unlink(BackingFile);
  return 0;
}

int main()
{

  int option;

  char name[50];
  char message[500];
  char enteredName[50];
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

  //Start Client
  printf("[%lu:C][%02d:%02d:%02d:%03i][] (Client has been started.)\n", pid, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, time_now.tv_usec);
  printf("To create a new client please press 1. If you want to exit, press 2\n");
  scanf("%d", &option);

  while (option < 2)
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

    // write message
    memoryWriter(message);

    printf("To create a new client please press 1. If you want to exit, press 2\n");
    scanf("%d", &option);
  }

  return 0;
}
