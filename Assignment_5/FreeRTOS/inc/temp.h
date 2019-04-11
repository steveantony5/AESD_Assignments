/*
 * temp.h
 *
 *  Created on: Apr 9, 2019
 *      Author: Steve Antony
 */

#ifndef TEMP_H_
#define TEMP_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "driverlib/gpio.h"
#include "driverlib/inc/hw_memmap.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"

#include "log.h"
#include "i2c.h"

/**********************************************
 *        MACRO
 **********************************************/
#define TEMP_THRESHOLD_HIGHER (28)
#define TEMP_THRESHOLD_LOWER  (23)

#define QUEUE_TIMEOUT_TICKS (10)
#define NOTIFY_TAKE_TIMEOUT (500)
#define TEMP_TIME_PERIOD_MS (1000)
#define TEMP_SENSOR_ADDR (0x48)
#define TEMP_REG_OFFSET_ADDR (0x00)
/**********************************************
 *        GLOBALS
 **********************************************/
struct log_struct_temp
{
    char time_stamp[30];
    char temp[40];

};

struct log_struct_led
{
    char time_stamp[30];
    long count;
    char name[10];

};


extern TaskHandle_t handle;

extern uint32_t output_clock_rate_hz;

extern QueueHandle_t myQueue_temp, myQueue_alert;

/**********************************************
 *        Function Prototypes
 **********************************************/
/********************************************
 * Func name :   TempTask
 * Parameters:   none
 * Description : Thread for temperature task
 ********************************************/
void TempTask(void *);

/********************************************
 * Func name :   vTimerCallback_Temp_handler
 * Parameters:   none
 * Description : handler for temperature timer
 ********************************************/
void vTimerCallback_Temp_handler( TimerHandle_t  *);

/********************************************
 * Func name :   i2c_setup
 * Parameters:   none
 * Description : Configuration of i2c bus
 ********************************************/
void i2c_setup(void);

/********************************************
 * Func name :   read_temp
 * Parameters:   slave address, register address
 * Description : reads the temperature
 ********************************************/
int16_t read_temp(uint8_t , uint8_t);

/********************************************
 * Func name :   AlertTask
 * Parameters:   none
 * Description : Thread for alert task
 ********************************************/
void AlertTask(void *);

#endif /* TEMP_H_ */
