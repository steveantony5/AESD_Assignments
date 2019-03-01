#include <stdio.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

void LED(void);


#define SH_SIZE (100)
#define SHM_KEY (0x2323)
#define BUFFER_SIZE (150)


#define  NOT_READY  -1
#define  FILLED     0
#define  TAKEN      1

struct shared_data
{
	int data_available;
	char message[SH_SIZE];
};

struct shared_data *shared_address;

int shmid;
char command[10];
char buffer[BUFFER_SIZE];


void hanler_kill_process2(int num)
{
	printf("Killed Process 2\n");
	FILE *log_handler;

	log_handler = fopen("log_SHM.txt","a+");
	if(log_handler == NULL)
	{
		printf("error on opening log handler file\n");
	}

			

	memset(buffer,0, BUFFER_SIZE);
    sprintf(buffer,"%d\tProcess 2 kill handler:Killed Process\n",(int)time(NULL));      
    fwrite(buffer, 1, strlen(buffer),log_handler);
	fclose(log_handler);
	
	if (shmdt(shared_address) == -1) 
	{
    	printf("shmdt error\n");
    }


    exit(0);

}

int main()
{

	signal(SIGINT,hanler_kill_process2);

	

	FILE *log;
	log = fopen("log_SHM.txt","a+");
	if(log == NULL)
	{
		printf("error on opening log file\n");
		return -1;
	}

	memset(buffer, 0, BUFFER_SIZE);

	sprintf(buffer,"%d\tProcess 2: PID - %d\t IPC method- Shared memory\n",(int)time(NULL),getpid());
	printf("%s",buffer);
	fwrite(buffer, 1, strlen(buffer),log);

	fclose(log);


	key_t key = SHM_KEY;


	shmid = shmget(key,sizeof(struct shared_data), 0644|IPC_CREAT);
	if(shmid <= 0)
	{
		printf("Error on creating shared memory\n");
	}
	else
	{
		printf("Created shared memory\n");
	}

	shared_address = (struct shared_data *)shmat(shmid, NULL, 0);

	if ( shared_address == (void *)-1)
	{
		printf("Attachement failed\n");
	}

	else
	{

		printf("Shared address %p\n",shared_address);
		
		while(1)
		{
			log = fopen("log_SHM.txt","a+");
			if(log == NULL)
			{
				printf("error on opening log file\n");
				return -1;
			}

			while (shared_address->data_available != FILLED)
				sleep(1);
		
			memset(buffer, 0, BUFFER_SIZE);
			sprintf(buffer,"%d\tProcess 2: The data received %s\n",(int)time(NULL),shared_address->message);
			printf("%s",buffer);
			fwrite(buffer, 1, strlen(buffer),log);

			sscanf(shared_address->message,"%s",command);
      		if(strcmp(command,"command") == 0)
      		{
        		LED();
      		}

      
      		shared_address->data_available = TAKEN;

      		printf("Enter the message to be sent to process 2\n");
			memset(shared_address->message, 0, SH_SIZE);
			fgets(shared_address->message , SH_SIZE, stdin);
			shared_address-> data_available = FILLED;
			printf("data available\n");

			memset(buffer, 0, BUFFER_SIZE);
			sprintf(buffer,"%d\tProcess 2: Sending message to process 1\n",(int)time(NULL));
			printf("%s",buffer);
			fwrite(buffer, 1, strlen(buffer),log);


			while (shared_address->data_available != TAKEN)
          		sleep(1);

          	fclose(log);


		}

		if (shmdt(shared_address) == -1) 
		{
      		printf("shmdt error\n");
      	}


      	if (shmctl(shmid, IPC_RMID, NULL) == -1)
      	{
      		printf("Error on deleting SHM\n");
      	}


	}




}




void LED()
{
    char LED_state[10];

    sscanf(shared_address->message,"%*s %s",LED_state);
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