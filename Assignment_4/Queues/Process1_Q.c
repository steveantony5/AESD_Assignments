/********************************************
              Includes
********************************************/
#include "Process_Q.h"


/********************************************
              Globals
********************************************/
char buffer[BUFFER_SIZE];
char message[BUFFER_SIZE];
mqd_t my_queue;
char command[10];

/********************************************
              Signal Handler
            For signal SIGINT
********************************************/

void hanler_kill_process1(int num)
{
        printf("Killed Process 1\n");
        FILE *log_handler;

        log_handler = fopen("log_Queue.txt","a+");
        if(log_handler == NULL)
        {
                printf("error on opening log handler file\n");
        }

                        

        memset(buffer,0, BUFFER_SIZE);
        sprintf(buffer,"%d\tProcess 1 kill handler:Killed Process due to signal %d\n",(int)time(NULL),num);      
        fwrite(buffer, 1, strlen(buffer),log_handler);

        fclose(log_handler);

   	    mq_close(my_queue);
		    mq_unlink(QUEUE_NAME);      

        exit(0);
}

/********************************************
              Main Function
********************************************/
int main()
{
    signal(SIGINT,hanler_kill_process1);

    FILE *log;
    log = fopen("log_Queue.txt","a+");
    if(log == NULL)
    {
        printf("error on opening log file\n");
        return -1;
    }

    memset(buffer, 0, BUFFER_SIZE);

    sprintf(buffer,"%d\tProcess 1: PID - %d\t IPC method- POSIX Queues\t Queue name %s\n",(int)time(NULL),getpid(),QUEUE_NAME);
    printf("%s",buffer);
    fwrite(buffer, 1, strlen(buffer),log);

    fclose(log);


     // initiate queue
	   if(queue_creation() != 0)
      {
        return 1;
      }

      while(1)
   	  {
   		  //opening the file in read mode
        log =fopen("log_Queue.txt","a+");
                                
        //checking whether the file exists in client folder
        if(log == NULL)
        {
          printf("Process1: File doesn't exits\n");
          return 1;
        }


   		

   		  memset(buffer, 0, BUFFER_SIZE);
        sprintf(buffer,"%d\tProcess 1: Sending message to process2\n",(int)time(NULL));
        fwrite(buffer, 1, strlen(buffer),log);

		    memset(message, 0, BUFFER_SIZE);
   		  printf("Enter the message to be sent to process 2\n");
   		  fgets(message,BUFFER_SIZE, stdin);

   		  mq_send(my_queue, message, BUFFER_SIZE, 0);
  
        memset(message, 0, BUFFER_SIZE);
  
        mq_receive(my_queue, message, BUFFER_SIZE, 0);
  
        //Check if any command for LED is received
        sscanf(message,"%s",command);
        if(strcmp(command,"command") == 0)
        {
          LED();
        }

   		   memset(buffer, 0, BUFFER_SIZE);
        sprintf(buffer,"%d\tProcess 1: The data received is %s\n",(int)time(NULL),message);
        fwrite(buffer, 1, strlen(buffer),log);
        printf("%s\n",buffer);

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

/********************************************
        Function for creating queue
********************************************/
int queue_creation()
{
    struct mq_attr attributes;

    attributes.mq_flags = 0;       /* Flags: 0 or O_NONBLOCK */
    attributes.mq_maxmsg = 10;      /* Max. # of messages on queue */
    attributes.mq_msgsize = BUFFER_SIZE;     /* Max. message size (bytes) */
    attributes.mq_curmsgs = 5;     /* # of messages currently in queue */

    my_queue = mq_open(QUEUE_NAME, O_CREAT | O_RDWR, 0666, &attributes);    
    if(my_queue == (mqd_t) -1)
    {
      printf("Error on queue creation\n");
      return 1;
    }
    return 0;
}