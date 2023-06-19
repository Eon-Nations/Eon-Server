#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define MAXLINE 1024
#define SOCKET_ERROR 10
#define SERVER_PORT "25565"
#define SERVER_BACKLOG 106

#define CHECK_ERROR(x) if (x < 0) { perror("Socket error"); exit(x); };

