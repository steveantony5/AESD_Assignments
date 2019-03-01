/*****************************************************************
						Includes
*****************************************************************/
#include "POSIX_Timer.h"


/*****************************************************************
					POSIX Timer Handler
			Prints CPU utilization every 100ms to the log file
*****************************************************************/

void my_timer_handler (union sigval val)
{

	char buffer[100];
	memset(buffer,0,sizeof(buffer));

	sprintf(buffer,"%d\tCPU Utilization %f\n",(int)time(NULL),CPU_utiliztion);
	printf("%s",buffer);

	FILE *log;
	log = fopen("log.txt","a+");
	if(log != NULL)
	{
		fwrite(buffer,1,strlen(buffer),log);
	}
	fclose(log);

	kick_timer(DELAY_NS);
}

/*****************************************************************
					POSIX Timer configuration
*****************************************************************/
int setup_timer_POSIX()
{
	struct 	sigevent sev;
	sev.sigev_notify = SIGEV_THREAD; //Upon timer expiration, invoke sigev_notify_function
	sev.sigev_notify_function = &my_timer_handler; //this function will be called when timer expires
	sev.sigev_notify_attributes = NULL;
	sev.sigev_value.sival_ptr = &timer_id;


	if(timer_create(CLOCK_REALTIME, &sev, &timer_id) != 0) //on success, timer id is placed in timer_id
	{
		printf("Error on creating timer\n");
	}  



    return 0;
}

/*****************************************************************
					Start configuration
			Parameter : delay in nano secs
*****************************************************************/
int kick_timer(int interval_ns)
{
   struct itimerspec in;

	in.it_value.tv_sec = 0;
    in.it_value.tv_nsec = interval_ns; //sets initial time period
    in.it_interval.tv_sec = 0;
    in.it_interval.tv_nsec = interval_ns; //sets interval
    
    //issue the periodic timer request here.
    int status = timer_settime(timer_id, 0, &in, NULL) ;
    if( status != 0)
    {
    	printf("Error on settime function\n");
    	return status;
    }
    return 0;
}

/*****************************************************************
					Destroy Timer
*****************************************************************/
int stop_timer()
{
	timer_delete(timer_id);

    return 0;
}


