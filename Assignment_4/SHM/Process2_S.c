/******************************************
			Include
******************************************/
#include "SHM.h"


/******************************************
			Macros
******************************************/
#define SH_SIZE (100)
#define SHM_KEY (0x2323)
#define BUFFER_SIZE (150)

#define  NOT_READY  -1
#define  FILLED     0
#define  TAKEN      1

/******************************************
			Globals
******************************************/
struct shared_data
{
	int data_available;
	char message[SH_SIZE];
};

struct shared_data *shared_address;

int shmid;
char command[10];
char buffer[BUFFER_SIZE];

/******************************************
			Signal Handler
		  For signal SIGINT
******************************************/
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
/******************************************
			   Globals
******************************************/
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

	//set up shared memory
	if(setup_SHM() != 0)
	{
		return 1;
	}

	memset(buffer, 0, BUFFER_SIZE);

	sprintf(buffer,"%d\tProcess 1: PID - %d\t IPC method- Shared memory\tSHM id %d\n",(int)time(NULL),getpid(),shmid);
	printf("%s",buffer);
	fwrite(buffer, 1, strlen(buffer),log);

	fclose(log);

	

		while(1)
		{
			log = fopen("log_SHM.txt","a+");
			if(log == NULL)
			{
				printf("error on opening log file\n");
				return -1;
			}

			//wait till the shared data is filled
			while (shared_address->data_available != FILLED)
				sleep(1);
		
			//reading the filled in data
			memset(buffer, 0, BUFFER_SIZE);
			sprintf(buffer,"%d\tProcess 2: The data received %s\n",(int)time(NULL),shared_address->message);
			printf("%s",buffer);
			fwrite(buffer, 1, strlen(buffer),log);

			//check if any command is received in message
			sscanf(shared_address->message,"%s",command);
      		if(strcmp(command,"command") == 0)
      		{
        		LED();
      		}
      

      		//set flag as data taken
      		shared_address->data_available = TAKEN;

      		//sending message to process 1
      		printf("Enter the message to be sent to process 2\n");
			memset(shared_address->message, 0, SH_SIZE);
			fgets(shared_address->message , SH_SIZE, stdin);
			//set the flag as filled
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


}


/******************************************
		Function for LED Control
******************************************/
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

/******************************************************
		Function for Setting up shared memory
******************************************************/
int setup_SHM()
{
	key_t key = SHM_KEY;


	//creating shared memory
	shmid = shmget(key,sizeof(struct shared_data), 0644|IPC_CREAT);
	if(shmid <= 0)
	{
		printf("Error on creating shared memory\n");
	}
	else
	{
		printf("Created shared memory\n");
	}

	//attaching the process to shared memory
	shared_address = (struct shared_data *)shmat(shmid, NULL, 0);

	if ( shared_address == (void *)-1)
	{
		printf("Attachement failed\n");
		return 1;
	}
	return 0;
}


