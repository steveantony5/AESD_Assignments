/*---------------------------------------------------------------------------------------------------------
 * File name : server.c
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
#define LISTEN_MAX (10)


/************************************
        Globals
************************************/
char buffer[BUFFERSIZE];
char message[BUFFERSIZE];
int new_socket, server_socket;
char command[10];

/************************************
        Signal Handler
      For signal SIGINT
************************************/
void hanler_kill_process2(int num)
{
    printf("Killed Process 2\n");
    FILE *log_handler;

    log_handler = fopen("log_SOCK.txt","a+");
    if(log_handler == NULL)
    {
        printf("error on opening log handler file\n");
    }

    memset(buffer,0, BUFFERSIZE);
    sprintf(buffer,"%d\tProcess 2 kill handler:Killed Process due to %d\n",(int)time(NULL),num);      
    fwrite(buffer, 1, strlen(buffer),log_handler);

    fclose(log_handler);

                   
    close(server_socket);
    close(new_socket);
    
    exit(0);
}

/************************************
        Main Function
************************************/
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

        int portno;// declaring a variable for port number
        portno = atoi(argv[1]);// storing the port number from command line argument

        if(socket_creation_server(portno)!=0)
        {
            return 1;
        }


        memset(buffer, 0, BUFFERSIZE);

        sprintf(buffer,"%d\tProcess 2: PID - %d\tIPC method- Sockets\tSocket id %d\n",(int)time(NULL),getpid(),server_socket);
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
                        printf("Process2: File doesn't exits\n");
                        return 1;
                }
                
                memset(message, 0, BUFFERSIZE);

                //receiving from the other process
                recv(new_socket,message ,BUFFERSIZE, 0);


                

                memset(buffer, 0, BUFFERSIZE);

                sprintf(buffer,"%d\tProcess 2: The data received %s\n",(int)time(NULL),message);
                fwrite(buffer, 1, strlen(buffer),log);
                printf("%s",buffer);

                //check if any command is received
                sscanf(message,"%s",command);
                if(strcmp(command,"command") == 0)
                {
                    LED();
                }


                memset(message, 0, BUFFERSIZE);

                printf("Enter the message to be sent to process 1\n");
                fgets(message , BUFFERSIZE, stdin);

                //sending to the other process
                send(new_socket, message, BUFFERSIZE , 0);
                memset(buffer, 0, BUFFERSIZE);

                sprintf(buffer,"%d\tProcess 2: Sending message to process1\n",(int)time(NULL));
                fwrite(buffer, 1, strlen(buffer),log);
                fclose(log);

        }


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

/**********************************************
       Function for server socket creation

       Parameters : Port number
***********************************************/
int socket_creation_server(int port)
{

        //creating the socket for client 

        server_socket = socket(AF_INET,SOCK_STREAM,0);// setting the client socket
        if(server_socket < 0 ) // enters this loop if port number is not given as command line argument
        {
                //printing error message when opening client socket
                printf("\nError opening server socket\n");
                return 1;
        }


        struct sockaddr_in server_address;

        memset(&server_address,0,sizeof(server_address));

        //assigning values for the server address structure
        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(port); // converting to network byte order
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
            return 1;
        }
        else
        {
            printf("established connection\n");
        }
        return 0;

}