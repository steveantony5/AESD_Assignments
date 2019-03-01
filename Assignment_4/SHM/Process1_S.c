/******************************************
			Include
******************************************/
#include "SHM.h"


/******************************************
			MACROS
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
char buffer[BUFFER_SIZE];


struct shared_data
{
	int data_available;
	char message[SH_SIZE];
};

struct shared_data *shared_address;
int shmid;
char command[10];


/******************************************
			Signal Handler
		  For Signal SIGINT
******************************************/
void hanler_kill_process1(int num)
{
	printf("Killed Process 1\n");
	FILE *log_handler;


	log_handler = fopen("log_SHM.txt","a+");
	if(log_handler == NULL)
	{
		printf("error on opening log handler file\n");
	}

			
 
	memset(buffer,0, BUFFER_SIZE);
    sprintf(buffer,"%d\tProcess 1 kill handler:Killed Process due to signal %d\n",(int)time(NULL),num);      
    fwrite(buffer, 1, strlen(buffer),log_handler);
	
	fclose(log_handler);

	if (shmdt(shared_address) == -1) 
	{
    	printf("shmdt error\n");
    }


    if (shmctl(shmid, IPC_RMID, NULL) == -1)
    {
    	printf("Error on deleting SHM\n");
    }

    exit(0);
}
/******************************************
			Main function
******************************************/
int main()
{

	signal(SIGINT,hanler_kill_process1);

	FILE *log;
	log = fopen("log_SHM.txt","a+");
	if(log == NULL)
	{
		printf("error on opening log file\n");
		return -1;
	}

	//setup Shared memory
	if(setup_SHM()!=0)
	{
		return 1;
	}

	

	memset(buffer, 0, BUFFER_SIZE);

	sprintf(buffer,"%d\tProcess 1: PID - %d\t IPC method- Shared memory\tSHM id %d\n",(int)time(NULL),getpid(),shmid);
	printf("%s",buffer);
	fwrite(buffer, 1, strlen(buffer),log);

	fclose(log);

	
		
		shared_address-> data_available = NOT_READY;

		while(1)
		{
			log = fopen("log_SHM.txt","a+");
			if(log == NULL)
			{
				printf("error on opening log file\n");
				return -1;
			}

			//sending data to the other process
			printf("Enter the message to be sent to process 2\n");
			memset(shared_address->message, 0, SH_SIZE);
			fgets(shared_address->message , SH_SIZE, stdin);
			//setting the flag as filled
			shared_address-> data_available = FILLED;
			printf("data available\n");


			memset(buffer, 0, BUFFER_SIZE);
			sprintf(buffer,"%d\tProcess 1: Sending message to process2\n",(int)time(NULL));
			printf("%s",buffer);
			fwrite(buffer, 1, strlen(buffer),log);

			

			//wait till the read is completed on the other process
			while (shared_address->data_available != TAKEN)
          		sleep(1);


          	//wait if the shared memory is already filled
          	while(shared_address->data_available != FILLED)
          		sleep(1);

          	memset(buffer, 0, BUFFER_SIZE);

          	//read data from the shared memory
			sprintf(buffer,"%d\tProcess 1: The data received %s\n",(int)time(NULL),shared_address->message);
			printf("%s",buffer);
			fwrite(buffer, 1, strlen(buffer),log);

			//check if any command is received
			sscanf(shared_address->message,"%s",command);
      		if(strcmp(command,"command") == 0)
      		{
        		LED();
      		}
      
      		//set the flag as the memory is read
      		shared_address->data_available = TAKEN;

      		fclose(log);
      	}

}



/******************************************************
		Function for LED Control
******************************************************/

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
		return 1;
	}
	return 0;
}