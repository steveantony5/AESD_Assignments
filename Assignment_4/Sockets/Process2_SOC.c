/*---------------------------------------------------------------------------------------------------------
 * File name : server.c
 * Author    : Steve Antony Xavier Kennedy
 *
 
 Command line inputs: <IP address> <port number>
 eg:
 ./client 127.0.0.1 9999
 --------------------------------------------------------------------------------------------------------*/
 

// Header section
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


#define BUFFERSIZE (200) 
#define LISTEN_MAX (10)

char buffer[BUFFERSIZE];
char message[BUFFERSIZE];
int new_socket, server_socket;
char command[10];

struct timeval t_obj;



    


void LED(void);

void hanler_kill_process2(int num)
{
    printf("Killed Process 2\n");
    FILE *log_handler;

    log_handler = fopen("log_SOCK.txt","a+");
    if(log_handler == NULL)
    {
        printf("error on opening log handler file\n");
    }

            

    fwrite("Process 2 kill handler:Killed Process\n", 1, strlen("Process 2 kill handler:Killed Process\n"),log_handler);

    fclose(log_handler);

                     
    close(server_socket);
    close(new_socket);
    
    exit(0);
}

int main(int argc, char *argv[])
{

        signal(SIGINT,hanler_kill_process2);

        if(argc<2)// passing ip address of server and port number as command line argument
        {
                printf("\n Please provide port number\n");
                exit(1);

        }

        FILE *log;
        log = fopen("log_SOCK.txt","a+");
        if(log == NULL)
        {
            printf("error on opening log file\n");
            return -1;
        }


        memset(buffer, 0, BUFFERSIZE);
        gettimeofday(&t_obj, NULL);

        sprintf(buffer,"%s Process 1: PID - %d\nProcess 1: IPC method- Sockets\n",ctime(&t_obj.tv_sec),getpid());
        printf("%s",buffer);
        fwrite(buffer, 1, strlen(buffer),log);

        fclose(log);



        //creating the socket for client 
        int portno;// declaring a variable for port number

        server_socket = socket(AF_INET,SOCK_STREAM,0);// setting the client socket
        if(server_socket < 0 ) // enters this loop if port number is not given as command line argument
        {
                //printing error message when opening client socket
                printf("\nError opening server socket\n");
                exit(1);
        }


        struct sockaddr_in server_address;

        memset(&server_address,0,sizeof(server_address));

        portno = atoi(argv[1]);// storing the port number from command line argument
        //assigning values for the server address structure
        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(portno); // converting to network byte order
        server_address.sin_addr.s_addr = INADDR_ANY;



        if(bind(server_socket,(struct sockaddr*)&server_address,sizeof(server_address))<0)
        {
            printf("Binding failed in the server");
            return 1;
        }

        /*Listening for clients*/
        if(listen(server_socket,LISTEN_MAX) < 0)
        {
            printf("Error on Listening ");
            return 1;
        }
        else
        {
            printf("\nlistening.....\n");
        }

       

        /*Accepting Client connection*/
        new_socket = 0;
        socklen_t clilen;
        struct sockaddr_in to_address;
        memset(&to_address,0,sizeof(to_address));


        clilen = sizeof(to_address);

        new_socket = accept(server_socket,(struct sockaddr*) &to_address, &clilen);
        if(new_socket<0)
        {
            perror("Error on accepting client");
            exit(1);
        }
        else
        {
            printf("established connection\n");
        }


        while(1)
        {       

                //opening the file in read mode
                log =fopen("log_SOCK.txt","a+");
                                
                //checking whether the file exists in client folder
                if(log == NULL)
                {
                        printf("Process2: File doesn't exits\n");
                        return 1;
                }
                
                memset(message, 0, BUFFERSIZE);

                recv(new_socket,message ,BUFFERSIZE, 0);

                printf("Process 2: Data received from Process 1\n\t%s\n",message);

                sscanf(message,"%s",command);
                if(strcmp(command,"command") == 0)
                {
                    LED();
                }

                memset(buffer, 0, BUFFERSIZE);
                sprintf(buffer,"Process 2: The data received %s\n",message);
                fwrite(buffer, 1, strlen(buffer),log);


                memset(message, 0, BUFFERSIZE);

                printf("Enter the message to be sent to process 1\n");
                fgets(message , BUFFERSIZE, stdin);

                send(new_socket, message, BUFFERSIZE , 0);
                memset(buffer, 0, BUFFERSIZE);
                sprintf(buffer,"Process 2: Sending message to process1\n");
                fwrite(buffer, 1, strlen(buffer),log);
                fclose(log);

        }


}


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