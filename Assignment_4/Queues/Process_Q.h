/********************************************
              Includes
********************************************/
#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

/********************************************
              Macros
********************************************/
#define QUEUE_NAME  "/my_queue"
#define BUFFER_SIZE (200)


/********************************************
              Function Prototypes
********************************************/
void LED(void);
int queue_creation(void);

