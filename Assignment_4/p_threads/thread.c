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
						Globals
*****************************************************************/
char CPU_time[10], idle_time[10];


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
	printf("Killed child 2\n");
	stop_timer();

	FILE *log;

	log = fopen("log.txt","a+");
	if(log == NULL)
		printf("Master: Error on opening file\n");

	fwrite("Child2 kill handler:Killed thread\n", 1, strlen("Child2 kill handler:Killed thread\n"),log);

	fclose(log);

	pthread_cancel(child2);

	exit(0);
}

void hanler_kill_child1(int num)
{
	printf("Killed child 1\n");

	FILE *log;

	log = fopen("log.txt","a+");
	if(log == NULL)
		printf("Master: Error on opening file\n");

	fwrite("Child1 kill handler:Killed thread\n", 1, strlen("Child1 kill handler:Killed thread\n"),log);

	fclose(log);

	pthread_cancel(child1);

	exit(0);
}


/*****************************************************************
						Child 1 thread
*****************************************************************/
void *character_histogram(void *arg)
{

	signal(SIGUSR1,hanler_kill_child1);
	signal(SIGUSR2,hanler_kill_child1);

	/*Gets the start time of thread*/
	clock_t begin = clock();

	printf("Child 1:Begin time %ld\n",(begin));

	files *ptr = (files *)arg;
	FILE *log, *input_file;


	log = fopen(ptr->filename_log,"a+");
	if(log == NULL)
		printf("Error on opening file\n");



	char buffer[50];


	printf("Child 1: Entered thread\n");
	int ch_count[26] = {0};

	printf("Child1: PID %ld\tLinux ID %d\n",pthread_self(),getpid());
	fwrite("Child 1: Entered thread\n", 1, strlen("Child 1: Entered thread\n"),log);

	sprintf(buffer,"Child1: PID %ld\tLinux ID %d\n",pthread_self(),getpid());
	fwrite(buffer, 1, strlen(buffer),log);

	input_file = fopen("input.txt","r");
	if(input_file == NULL)
		printf("Child 1: input.txt doesn't exists\n");

	char ch;
	while((ch = fgetc(input_file)) != EOF)
	{
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
		printf(" %c|%c - %d\n",(index+65),(index+97),ch_count[index]);

		if(ch_count[index] < 100)
		{
			memset(buffer,0,sizeof(buffer));
			sprintf(buffer,"%c|%c - %d\n",(index+65),(index+97),ch_count[index]);
			fwrite(buffer,1,strlen(buffer),log);
		}
	}


	fclose(log);

	fclose(input_file);

	clock_t exit = clock();

	printf("Child 1: Exit time %ld\n",(exit));

	pthread_cancel(child1);

	return 0;

}

/*****************************************************************
						master thread
*****************************************************************/
void * master_thread(void *args)
{
	files *ptr = (files *)args;

	printf("Master:Entered thread\n");

	FILE *log;

	log = fopen(ptr->filename_log,"a+");
	if(log == NULL)
		printf("Master: Error on opening file\n");

	fwrite("Master:Entered thread\n", 1, strlen("Master:Entered thread\n"),log);
	char buffer[50];

	printf("master: PID %ld\tLinux ID %d\n",pthread_self(),getpid());

	sprintf(buffer,"master: PID %ld\tLinux ID %d\n",pthread_self(),getpid());
	fwrite(buffer, 1, strlen(buffer),log);

	fclose(log);

	pthread_cancel(master);

	return 0;
}



void * CPU_time_utilization(void *args)
{

	printf("Child2:Entered thread\n");
	/*Gets the start time of thread*/
	clock_t begin = clock();

	printf("Child 2:Begin time %ld\n",(begin));

	files *ptr = (files *)args;

	FILE *log = NULL,*proc = NULL;

	log = fopen(ptr->filename_log,"a+");
	if(log == NULL)
		printf("Child2: Error on opening file\n");

	fwrite("Child2:Entered thread\n", 1, strlen("Child2:Entered thread\n"),log);

	char buffer[50];

	printf("Child2: PID %ld\tLinux ID %d\n",pthread_self(),getpid());

	sprintf(buffer,"Child2: PID %ld\tLinux ID %d\n",pthread_self(),getpid());
	fwrite(buffer, 1, strlen(buffer),log);


	fclose(log);

	
	signal(SIGUSR1,hanler_kill_child2);
	signal(SIGUSR2,hanler_kill_child2);

	kick_timer(100000000);

	
	while(1)
	{
		proc = fopen("/proc/stat","r");
		if(proc == NULL)
			printf("Child 2:Error on opening proc/stat file\n");

			
		//cpu  39388 701 46592 4973982
		fscanf(proc,"%*s %s %*s %*s %s",CPU_time, idle_time);
		fclose(proc);

	}



	return 0;

}



int main(int argc, char **argv)
{
	if(argc < 2)
	{
		printf("Enter log filename\n");
		return 1;
	}

	setup_timer_POSIX(100000000);

	

	strcpy(file_obj.filename_log,argv[1]);

	pthread_attr_t attr;
	pthread_attr_init(&attr); 

	pthread_create(&master, &attr, master_thread, &file_obj);	

	pthread_create(&child1, &attr, character_histogram, &file_obj);		

	pthread_create(&child2, &attr, CPU_time_utilization, &file_obj);  

	pthread_join(child1,NULL);
	pthread_join(child2,NULL);

	return 0;

}