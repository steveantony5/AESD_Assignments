/*
 * log.c
 *
 *  Created on: Apr 8, 2019
 *      Author: Steve Antony
 */
#include "log.h"


/**********************************************
 *        Global definitions
 **********************************************/
struct log_struct_temp log_temp_recv;
struct log_struct_led log_led_recv;



void queue_init()
{
    myQueue_temp = xQueueCreate(QueueLength, sizeof(log_temp_recv));
    if(myQueue_temp == NULL)
    {
        UARTprintf("error on queue creation myQueue_temp\n");
    }

    myQueue_led = xQueueCreate(QueueLength, sizeof(log_led_recv));
    if(myQueue_led == NULL)
    {
        UARTprintf("error on queue creation myQueue_led\n");
    }

    myQueue_alert = xQueueCreate(QueueLength, sizeof(uint32_t));
    if(myQueue_alert == NULL)
    {
        UARTprintf("error on queue creation myQueue_alert\n");
    }

}

/**********************************************
 *        Logger thread
 **********************************************/
void LogTask(void *pvParameters)
{
    int temp_notf;

    for(;;)
    {
        if(xQueueReceive(myQueue_led, &log_led_recv, TIMEOUT_TICKS ) == pdTRUE )
            UARTprintf("[%s]--->LED   name: %s toggle count: %d\n",log_led_recv.time_stamp,log_led_recv.name,log_led_recv.count);

        if(xQueueReceive(myQueue_temp, &log_temp_recv, TIMEOUT_TICKS ) == pdTRUE )
            UARTprintf("[%s]--->Temp  temp: %s\n",log_temp_recv.time_stamp,log_temp_recv.temp);

        if(xQueueReceive(myQueue_alert, &temp_notf, TIMEOUT_TICKS ) == pdTRUE )
            UARTprintf("------------->Temp out of range - notified\n");

    }
}



