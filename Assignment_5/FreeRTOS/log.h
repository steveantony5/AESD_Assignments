/*
 * log.h
 *
 *  Created on: Apr 8, 2019
 *      Author: Steve Antony
 */

#ifndef LOG_H_
#define LOG_H_

/**********************************************
 *        Includes
 **********************************************/
#include "FreeRTOS.h"
#include "queue.h"
#include "portmacro.h"
#include "utils/uartstdio.h"
#include "portmacro.h"
#include "time.h"
#include "temp.h"

/**********************************************
 *        MACRO
 **********************************************/
#define QueueLength (100)
#define TIMEOUT_TICKS (10)
/**********************************************
 *        Global declarations
 **********************************************/
extern QueueHandle_t myQueue_temp, myQueue_led, myQueue_alert;

/**********************************************
 *        Function Prototypes
 **********************************************/
void queue_init();
void LogTask(void *pvParameters);

#endif /* LOG_H_ */
