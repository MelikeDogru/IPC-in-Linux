#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <time.h>
#include <semaphore.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include "socket.h"

#define AccessPermsSemp 0644

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
  int socketfiled = socket(AF_INET, SOCK_STREAM, 0);
  if (socketfiled < 0)
  {
    report_and_terminate("socket", 1);
  }

  //Host
  struct hostent *hostpointer = gethostbyname(Host);
  if (!hostpointer)
  {
    report_and_terminate("gethostbyname", 1);
  }
  if (hostpointer->h_addrtype != AF_INET)
  {
    report_and_terminate("bad address family", 1);
  }

  //Server
  struct sockaddr_in saddress;
  memset(&saddress, 0, sizeof(saddress));
  saddress.sin_family = AF_INET;
  saddress.sin_addr.s_addr = ((struct in_addr *)hostpointer->h_addr_list[0])->s_addr;
  saddress.sin_port = htons(PortNumber);

  if (connect(socketfiled, (struct sockaddr *)&saddress, sizeof(saddress)) < 0)
  {
    report_and_terminate("connect", 1);
  }

  char name[50];
  char message[800];
  char enteredName[50];
  char enteredMessage[800];

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
  for (int i = 0; i < 10; i++)
  {
    printf("[%lu:C][%02d:%02d:%02d:%03i][%s] Please write your message:\n", pid, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, time_now.tv_usec, name);
    scanf(" %[^\t\n]s", &enteredMessage);
    //scanf("%s", enteredMessage);
    strcpy(message, enteredMessage);

    //Show entered message
    printf("[%lu:C][%02d:%02d:%02d:%03i][%s] (Tring to write message: “%s”)\n", pid, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, time_now.tv_usec, name, message);

    // write message

    if (strcmp(message, "killme") == 0)
    {
      printf("[%lu:C][%02d:%02d:%02d:%03i][%s] (Client is terminated!)\n", pid, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, time_now.tv_usec, name);
      kill(pid, SIGTERM);
      break;
    }
    
    if (!sem_wait(semaphoreWrite))
    {
    if (write(socketfiled, message, strlen(message)) > 0)
    {
      char buff[BufferSize + 1];
      memset(buff, '\0', sizeof(buff));
      if (read(socketfiled, buff, sizeof(buff)) > 0)
        puts(buff);
    }

    sem_post(semaphoreWrite);
    }
  }

  sem_post(semaphoreWrite);

  close(socketfiled);
  sem_close(semaphoreWrite);

  return 0;
}
