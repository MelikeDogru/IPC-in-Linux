#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define FileName "design1.dat"

// To report error and exit
void report_and_exit(const char *message)
{
  perror(message);
  exit(-1);
}

// To produce the message
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
    report_and_exit("Unlockin process is failed.");

  close(fileDesc);
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
    producer(message);

    printf("To create a new client please press 1. If you want to exit, press 2\n");
    scanf("%d", &option);
  }

  return 0;
}
