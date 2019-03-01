/*---------------------------------------------------------------------------------------------------------
 * File name : client.c
 * Author    : Steve Antony Xavier Kennedy
 *
 
 Command line inputs: <IP address> <port number>
 eg:
 ./client 127.0.0.1 9999
 --------------------------------------------------------------------------------------------------------*/
 

/************************************
        Includes
************************************/
#include "sockets.h"

/************************************
        Macros
************************************/
#define BUFFERSIZE (200) 

/************************************
        Globals
************************************/
char buffer[BUFFERSIZE];
char message[BUFFERSIZE];
int client_socket;
char command[10];

/************************************
        Signal Handler
      For Signal SIGINT
************************************/
void hanler_kill_process1(int num)
{
        printf("Killed Process 1\n");
        FILE *log_handler;

        log_handler = fopen("log_SOCK.txt","a+");
        if(log_handler == NULL)
        {
                printf("error on opening log handler file\n");
        }

        memset(buffer,0, BUFFERSIZE);
        sprintf(buffer,"%d\tProcess 1 kill handler:Killed Process due to signal %d\n",(int)time(NULL),num);      
        fwrite(buffer, 1, strlen(buffer),log_handler);

        fclose(log_handler);

        close(client_socket);
      
        exit(0);
}

/************************************
        Main Function
************************************/
int main(int argc, char *argv[])
{
        signal(SIGINT,hanler_kill_process1);

        if(argc<3)// passing ip address of server and port number as command line argument
        {
                printf("\n Please provide host ip address, port number\n");
                exit(1);

        }

        FILE *log;
        log = fopen("log_SOCK.txt","a+");
        if(log == NULL)
        {
            printf("error on opening log file\n");
            return -1;
        }

        
        //creating the socket for client 
        int portno;// declaring a variable for port number
        portno = atoi(argv[2]);// storing the port number from command line argument

        if(socket_creation_client(portno,argv[1]) != 0)
        {
            return 1;
        }

        memset(buffer, 0, BUFFERSIZE);

        sprintf(buffer,"%d\tProcess 1: PID - %d\tIPC method- Sockets\tSocket id %d\n",(int)time(NULL),getpid(),client_socket);
        printf("%s",buffer);
        fwrite(buffer, 1, strlen(buffer),log);

        fclose(log);
        
        while(1)
        {       

                //opening the file in read mode
                log =fopen("log_SOCK.txt","a+");
                                
                //checking whether the file exists in client folder
                if(log == NULL)
                {
                        printf("Process1: File doesn't exits\n");
                        return 1;
                }
                
                memset(message, 0, BUFFERSIZE);
                printf("Enter the message to be sent to process 2\n");

                fgets(message , BUFFERSIZE, stdin);

                //send the message to the other process
                send(client_socket, message, BUFFERSIZE , 0);

                memset(buffer, 0, BUFFERSIZE);
                sprintf(buffer,"%d\tProcess 1: Sending message to process2\n",(int)time(NULL));
                fwrite(buffer, 1, strlen(buffer),log);

                memset(message, 0, BUFFERSIZE);

                //receive the message from the other process
                recv(client_socket,message ,BUFFERSIZE, 0);

                memset(buffer, 0, BUFFERSIZE);
                sprintf(buffer,"%d\tProcess 1: The data received is %s\n",(int)time(NULL),message);
                fwrite(buffer, 1, strlen(buffer),log);
                printf("%s",buffer);

                //check if an command is received
                sscanf(message,"%s",command);
                if(strcmp(command,"command") == 0)
                {
                    LED();
                }
                fclose(log);

        }
              
close(client_socket);
}

/************************************
       Function for LED control
************************************/
void LED()
{
    char LED_state[10];

    sscanf(message,"%*s %s",LED_state);
    if(strcmp(LED_state,"ON")==0)
    {
        printf("Turned on the LED\n");
    }
    else if(strcmp(LED_state,"OFF")==0)
    {
        printf("Turned off the LED\n");
    }
    else
    {
        printf("Invalid command received\n");
    }
}
/******************************************************
       Function for client socket creation
       Parameters : port number, ip address in string
*******************************************************/
int socket_creation_client(int port,char ip[20])
{
    client_socket = socket(AF_INET,SOCK_STREAM,0);// setting the client socket

    if(client_socket < 0 ) // enters this loop if port number is not given as command line argument
    {
        //printing error message when opening client socket
        printf("\nError opening client socket\n");
        return 1;
    }

    struct sockaddr_in server_address ;
    memset(&server_address,0,sizeof(server_address));
    //assigning values for the server address structure
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port); // converting to network byte order
    server_address.sin_addr.s_addr = inet_addr(ip);


    if(connect(client_socket,(struct sockaddr *) &server_address, sizeof(server_address)) < 0)
    {
        printf("Error on connect\n");
        return 1;
    }

    return 0;

}
