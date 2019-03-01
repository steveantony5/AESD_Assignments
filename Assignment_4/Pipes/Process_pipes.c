/*****************************************************
                Includes
*****************************************************/
#include "Process_pipes.h"

/*****************************************************
                Macros
*****************************************************/
#define BUFFERSIZE (200)

/*****************************************************
                Globals
*****************************************************/
char buffer[BUFFERSIZE];
char message[BUFFERSIZE];
char command[10];
int pipe_fd_1[2], pipe_fd_2[2];


/*****************************************************
                Globals
*****************************************************/

/*****************************************************
                Parent signal handler
            Handler for SIGNAL SIGINT
*****************************************************/
void hanler_kill_Parent(int num)
{
        FILE *log_handler;

        log_handler = fopen("log_Pipes.txt","a+");
        if(log_handler == NULL)
        {
                printf("error on opening log handler file\n");
        }

                        

        memset(buffer,0, BUFFERSIZE);
        sprintf(buffer,"%d\tParent kill handler:Killed Process due to signal number %d\n",(int)time(NULL),num);      
        fwrite(buffer, 1, strlen(buffer),log_handler);
        printf("%s",buffer);

        fclose(log_handler);

   		close(pipe_fd_1[1]);
   		close(pipe_fd_2[0]);


        exit(0);
}

/*****************************************************
                Child signal handler
            Handler for SIGNALS SIGINT
*****************************************************/
void hanler_kill_Child(int num)
{
        FILE *log_handler;

        log_handler = fopen("log_Pipes.txt","a+");
        if(log_handler == NULL)
        {
                printf("error on opening log handler file\n");
        }

                        

        memset(buffer,0, BUFFERSIZE);
        sprintf(buffer,"%d\tChild kill handler:Killed Process due to signal number %d\n",(int)time(NULL),num);     
        fwrite(buffer, 1, strlen(buffer),log_handler);
        printf("%s",buffer);

        fclose(log_handler);

      	close(pipe_fd_1[0]);
   		close(pipe_fd_2[1]);

        exit(0);
}

/*****************************************************
                Main function
*****************************************************/
int main()
{
	int pid;
	FILE *log;

	//creating the pipe from parent to child
	if(pipe(pipe_fd_1) == -1)
	{
		printf("Error on pipe creation pipe_fd_1\n");
		return 1;
	}

	//creating the pipe from child to parent
	if(pipe(pipe_fd_2) == -1)
	{
		printf("Error on pipe creation pipe_fd_2\n");
		return 1;
	}

    //forking to create a new child process
	pid = fork();

    //fork error
	if(pid < 0)
	{
		printf("failed on forking\n");
		return 1;
	}

	//parent process
	if(pid > 0)
	{
        signal(SIGINT,hanler_kill_Parent);

		while(1)
		{
			    //opening the file in read mode
                log =fopen("log_Pipes.txt","a+");
                                
                //checking whether the file exists in client folder
                if(log == NULL)
                {
                        printf("Parent: File doesn't exits\n");
                        return 1;
                }
                
                memset(message, 0, BUFFERSIZE);
                printf("Parent: Enter the message to be sent to child\n");

                fgets(message , BUFFERSIZE, stdin);
                write(pipe_fd_1[1], message, BUFFERSIZE);

                memset(buffer, 0, BUFFERSIZE);
                sprintf(buffer,"%d\tParent: Sending message to child\n",(int)time(NULL));
                fwrite(buffer, 1, strlen(buffer),log);

                memset(message, 0, BUFFERSIZE);

                read(pipe_fd_2[0], message, BUFFERSIZE);

                memset(buffer, 0, BUFFERSIZE);
                sprintf(buffer,"%d\tParent: The data received from child is %s\n",(int)time(NULL),message);
                fwrite(buffer, 1, strlen(buffer),log);
                printf("%s",buffer);

                sscanf(message,"%s",command);
                if(strcmp(command,"command") == 0)
                {
                    LED();
                }
                fclose(log);
		}
	}

    //child process
	else if(pid == 0)
	{
        signal(SIGINT,hanler_kill_Child);

		while(1)
		{
			    //opening the file in read mode
                log =fopen("log_Pipes.txt","a+");
                                
                //checking whether the file exists in client folder
                if(log == NULL)
                {
                        printf("Parent: File doesn't exits\n");
                        return 1;
                }

				memset(message, 0, BUFFERSIZE);

                read(pipe_fd_1[0], message, BUFFERSIZE);                

                memset(buffer, 0, BUFFERSIZE);
                sprintf(buffer,"%d\tChild: The data received from Parent is %s\n",(int)time(NULL),message);
                fwrite(buffer, 1, strlen(buffer),log);
                printf("%s",buffer);

                sscanf(message,"%s",command);
                if(strcmp(command,"command") == 0)
                {
                    LED();
                }

                memset(message, 0, BUFFERSIZE);
                printf("Child: Enter the message to be sent to Parent\n");

                fgets(message , BUFFERSIZE, stdin);
                write(pipe_fd_2[1], message, BUFFERSIZE);

                memset(buffer, 0, BUFFERSIZE);
                sprintf(buffer,"%d\tChild: Sending message to Parent\n",(int)time(NULL));
                fwrite(buffer, 1, strlen(buffer),log);

                fclose(log);


		}
	}


}

/*****************************************************
                LED Control function
*****************************************************/
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