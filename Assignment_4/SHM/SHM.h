/*****************************************
			Includes
*****************************************/
#include <stdio.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

/*****************************************
			Function Prototypes
*****************************************/
void LED(void);
int setup_SHM(void);