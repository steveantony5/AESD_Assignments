#ifndef POSIX_Timer_H_
#define POSIX_Timer_H_
/*****************************************************************
						Includes
*****************************************************************/
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>


/*****************************************************************
						Function Protypes
*****************************************************************/
int kick_timer(int);
int setup_timer_POSIX();
int stop_timer();

/*****************************************************************
						Globals
*****************************************************************/
extern float CPU_utiliztion;
extern timer_t timer_id;

/*****************************************************************
						MACROS
*****************************************************************/
#define DELAY_NS (100000000)


#endif /* POSIX_Timer_H_ */
