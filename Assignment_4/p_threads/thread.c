/*****************************************************************
						Includes
*****************************************************************/

#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <stdlib.h>
#include "POSIX_Timer.h"
#include <sys/types.h>
#include <unistd.h>

/*****************************************************************
						Macros
*****************************************************************/
#define BUFFERSIZE (500)
#define Delay_NS (100000000) //100 ms
/*****************************************************************
						Globals
*****************************************************************/
float CPU_utiliztion;

//structure to pass as argument to threads
typedef struct
{
	char filename_log[10];
}files;


files file_obj;

timer_t timer_id;

pthread_t child1;
pthread_t child2;
pthread_t master;

/*****************************************************************
						Thread signal handler
*****************************************************************/
void hanler_kill_child2(int num)
{
	char buffer[BUFFERSIZE];

	stop_timer();

	FILE *log_handler;

	log_handler = fopen("log.txt","a+");
	if(log_handler == NULL)
		printf("Master: Error on opening file\n");

	memset(buffer,0, BUFFERSIZE);
    sprintf(buffer,"%d\tChild2 kill handler:Killed thread due to signal %d\n",(int)time(NULL),num);      
    fwrite(buffer, 1, strlen(buffer),log_handler);
    printf("%s",buffer);

	fclose(log_handler);

	pthread_cancel(child2);

	exit(0);
}

void hanler_kill_child1(int num)
{
	char buffer[BUFFERSIZE];

	FILE *log_handler;

	log_handler = fopen("log.txt","a+");
	if(log_handler == NULL)
		printf("Master: Error on opening file\n");

	memset(buffer,0, BUFFERSIZE);
    sprintf(buffer,"%d\tChild1 kill handler:Killed thread due to signal %d\n",(int)time(NULL),num);      
    fwrite(buffer, 1, strlen(buffer),log_handler);
    printf("%s",buffer);

	fclose(log_handler);

	pthread_cancel(child1);

	exit(0);
}


/*****************************************************************
						Child 1 thread
*****************************************************************/
void *character_histogram(void *args)
{
	files *ptr = (files *)args;
	FILE *log_1, *input_file;

	log_1 = fopen(ptr->filename_log,"a+");
	if(log_1 == NULL)
	{
		printf("Error on opening file\n");
	}

	char buffer[BUFFERSIZE];

	memset(buffer,0, BUFFERSIZE);
	sprintf(buffer,"%d\tChild 1: Entered thread\n",(int)time(NULL));
	fwrite(buffer, 1, strlen(buffer),log_1);
	printf("%s",buffer);

	//signal handlers
	signal(SIGUSR1,hanler_kill_child1);
	signal(SIGUSR2,hanler_kill_child1);


	int ch_count[26] = {0};



	memset(buffer,0, BUFFERSIZE);
	sprintf(buffer,"%d\tChild1: PID %ld\tLinux ID %d\n",(int)time(NULL),pthread_self(),getpid());
	fwrite(buffer, 1, strlen(buffer),log_1);
	printf("%s",buffer);

	input_file = fopen("input.txt","r");
	if(input_file == NULL)
		printf("Child 1: input.txt doesn't exists\n");

	char ch;

	//count the characters
	while(!feof(input_file))
	{
		ch = fgetc(input_file);
	
		if((ch >= 65) && (ch <=90))
		{
			ch_count[ch - 65] += 1;
		}
		if((ch >= 97) && (ch <=122))
		{
			ch_count[ch - 97] += 1;
		}
	}


	//display the count
	for(int index = 0; index<26;index++)
	{
		//display characters that occured less than 100 times
		if(ch_count[index] < 100)
		{
			memset(buffer,0,BUFFERSIZE);
			sprintf(buffer,"%d\t%c|%c - %d\n",(int)time(NULL),(index+65),(index+97),ch_count[index]);
			fwrite(buffer,1,strlen(buffer),log_1);
			printf("%s",buffer);
		}
	}


	

	memset(buffer,0, BUFFERSIZE);
    sprintf(buffer,"%d\tChild 1: Thread Exited\n",(int)time(NULL));      
    fwrite(buffer, 1, strlen(buffer),log_1);
    printf("%s",buffer);


	
	fclose(log_1);

	fclose(input_file);

	pthread_cancel(child1);



	return 0;

}

/*****************************************************************
						master thread
*****************************************************************/
void * master_thread(void *args)
{

	files *ptr = (files *)args;


	FILE *log;

	log = fopen(ptr->filename_log,"a+");
	if(log == NULL)
		printf("Master: Error on opening file\n");


	char buffer[BUFFERSIZE];

	memset(buffer,0, BUFFERSIZE);
    sprintf(buffer,"%d\tMaster:Entered thread\n",(int)time(NULL));      
    fwrite(buffer, 1, strlen(buffer),log);
    printf("%s",buffer);

	
	memset(buffer,0, BUFFERSIZE);
	sprintf(buffer,"%d\tmaster: PID %ld\tLinux ID %d\n",(int)time(NULL),pthread_self(),getpid());
	fwrite(buffer, 1, strlen(buffer),log);
	printf("%s",buffer);

	memset(buffer,0, BUFFERSIZE);
    sprintf(buffer,"%d\tMaster: Thread Exited\n",(int)time(NULL));      
    fwrite(buffer, 1, strlen(buffer),log);
    printf("%s",buffer);


	fclose(log);

	pthread_cancel(master);

	return 0;
}


/*****************************************************************
						Child 2 thread
*****************************************************************/
void * CPU_time_utilization(void *args)
{

	char buffer[BUFFERSIZE];

	char CPU_time[20];
	char nice_time[20];
	char system_time[20];
	char idle_time[20];

	float CPU_time_int;
	float idle_time_int;
	float nice_time_int;
	float system_time_int;

	files *ptr = (files *)args;

	FILE *log_2 = NULL,*proc = NULL;

	log_2 = fopen(ptr->filename_log,"a+");
	if(log_2 == NULL)
		printf("Child2: Error on opening file\n");



	memset(buffer,0, BUFFERSIZE);
    sprintf(buffer,"%d\tChild2:Entered thread\n",(int)time(NULL));      
    fwrite(buffer, 1, strlen(buffer),log_2);
    printf("%s",buffer);

	

	memset(buffer,0, BUFFERSIZE);
	sprintf(buffer,"%d\tChild2: PID %ld\tLinux ID %d\n",(int)time(NULL),pthread_self(),getpid());
	fwrite(buffer, 1, strlen(buffer),log_2);
	printf("%s",buffer);


	fclose(log_2);

	//signal handlers
	signal(SIGUSR1,hanler_kill_child2);
	signal(SIGUSR2,hanler_kill_child2);

	kick_timer(Delay_NS);



	
	while(1)
	{
		memset(CPU_time,0,sizeof(CPU_time));
		memset(nice_time,0,sizeof(nice_time));
		memset(system_time,0,sizeof(system_time));
		memset(idle_time,0,sizeof(idle_time));

		proc = fopen("/proc/stat","r");
		if(proc == NULL)
			printf("Child 2:Error on opening proc/stat file\n");

			
		//cpu  39388 701 46592 4973982
		fscanf(proc,"%*s %s %s %s %s",CPU_time,nice_time,system_time, idle_time);

		CPU_time_int = atoi (CPU_time);
		idle_time_int = atoi(idle_time);
		nice_time_int = atoi(nice_time);
		system_time_int = atoi(system_time);

		CPU_utiliztion = (CPU_time_int + nice_time_int + system_time_int)/ \
									(CPU_time_int + nice_time_int + system_time_int+ idle_time_int);


		fclose(proc);

	}



	return 0;

}

/*****************************************************************
						Main Function
*****************************************************************/

int main(int argc, char **argv)
{
	if(argc < 2)
	{
		printf("Enter log filename\n");
		return 1;
	}

	setup_timer_POSIX();

	
	//getting the log filename
	strcpy(file_obj.filename_log,argv[1]);

	pthread_attr_t attr;
	pthread_attr_init(&attr); 

	pthread_create(&master, &attr, master_thread, &file_obj);	

	pthread_create(&child1, &attr, character_histogram, &file_obj);		

	pthread_create(&child2, &attr, CPU_time_utilization, &file_obj);  


	//wait till the child completes
	pthread_join(child1,NULL);
	pthread_join(child2,NULL);

	return 0;

}