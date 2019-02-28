#include <stdio.h>
#include <unistd.h> 
#include <string.h>
#include <signal.h>
#include <stdlib.h>

#define BUFFERSIZE (200)


void LED(void);

char buffer[BUFFERSIZE];
char message[BUFFERSIZE];
char command[10];

int pipe_fd_1[2], pipe_fd_2[2];

void hanler_kill_Parent(int num)
{
        printf("Killed Parent\n");
        FILE *log_handler;

        log_handler = fopen("log_Pipes.txt","a+");
        if(log_handler == NULL)
        {
                printf("error on opening log handler file\n");
        }

                        

        fwrite("Parent kill handler:Killed Process\n", 1, strlen("Parent kill handler:Killed Process\n"),log_handler);

        fclose(log_handler);

   		close(pipe_fd_1[1]);
   		close(pipe_fd_2[0]);


        exit(0);
}

void hanler_kill_Child(int num)
{
        printf("Killed Child\n");
        FILE *log_handler;

        log_handler = fopen("log_Pipes.txt","a+");
        if(log_handler == NULL)
        {
                printf("error on opening log handler file\n");
        }

                        

        fwrite("Child kill handler:Killed Process\n", 1, strlen("Child kill handler:Killed Process\n"),log_handler);

        fclose(log_handler);

      	close(pipe_fd_1[0]);
   		close(pipe_fd_2[1]);

        exit(0);
}

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

	pid = fork();

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
                sprintf(buffer,"Parent: Sending message to child\n");
                fwrite(buffer, 1, strlen(buffer),log);

                memset(message, 0, BUFFERSIZE);

                read(pipe_fd_2[0], message, BUFFERSIZE);
                printf("Parent: Data received from child\n\t%s\n",message);

                memset(buffer, 0, BUFFERSIZE);
                sprintf(buffer,"Parent: The data received from child\n\t%s\n",message);
                fwrite(buffer, 1, strlen(buffer),log);

                sscanf(message,"%s",command);
                if(strcmp(command,"command") == 0)
                {
                    LED();
                }
                fclose(log);
		}
	}
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
                printf("Child: Data received from Parent\n\t%s\n",message);

                sscanf(message,"%s",command);
                if(strcmp(command,"command") == 0)
                {
                    LED();
                }

                memset(buffer, 0, BUFFERSIZE);
                sprintf(buffer,"Child: The data received from Parent\n\t%s\n",message);
                fwrite(buffer, 1, strlen(buffer),log);

                memset(message, 0, BUFFERSIZE);
                printf("Child: Enter the message to be sent to Parent\n");

                fgets(message , BUFFERSIZE, stdin);
                write(pipe_fd_2[1], message, BUFFERSIZE);

                memset(buffer, 0, BUFFERSIZE);
                sprintf(buffer,"Child: Sending message to Parent\n");
                fwrite(buffer, 1, strlen(buffer),log);

                fclose(log);


		}
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