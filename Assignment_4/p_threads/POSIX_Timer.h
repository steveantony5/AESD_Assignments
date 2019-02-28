#ifndef POSIX_Timer_H_
#define POSIX_Timer_H_

#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>


int kick_timer(int);
int setup_timer_POSIX(int);
int stop_timer();


#endif /* POSIX_Timer_H_ */
