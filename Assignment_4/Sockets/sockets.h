/************************************
        Includes
************************************/
#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<unistd.h>
#include<ctype.h>
#include<netdb.h>
#include<string.h>
#include<arpa/inet.h>
#include<signal.h>
#include <sys/time.h>
#include <time.h>

/************************************
        Function declarations
************************************/
void LED(void);
int socket_creation_server(int port);
int socket_creation_client(int port,char ip[20]);