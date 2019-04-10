/* FreeRTOS 8.2 Tiva Demo
 *
 * main.c
 *
 * Steve Antony
 *
 * This is a simple demonstration project of FreeRTOS 8.2 on the Tiva Launchpad
 * EK-TM4C1294XL.  TivaWare driverlib sourcecode is included.
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "main.h"
#include "drivers/pinout.h"
#include "utils/uartstdio.h"


// TivaWare includes
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"

// FreeRTOS includes
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "driverlib/gpio.h"
#include "driverlib/inc/hw_memmap.h"
#include "log.h"
#include "temp.h"


/**********************************************
 *        Function Prototypes
 **********************************************/
void LEDTask(void *pvParameters);
void vTimerCallback_LED_handler( TimerHandle_t  *pxTimer );
QueueHandle_t myQueue_temp, myQueue_led, myQueue_alert;
struct log_struct_led log_led;
uint32_t output_clock_rate_hz;
TaskHandle_t handle;


/**********************************************
 *        Globals
 **********************************************/
int FLAG_LED = pdFALSE;
QueueHandle_t myQueue_temp, myQueue_led, myQueue_alert;
struct log_struct_led log_led;
uint32_t output_clock_rate_hz;
TaskHandle_t handle;

/**********************************************
 *        Main Function
 **********************************************/
int main(void)
{
    // Initialize system clock to 120 MHz
    output_clock_rate_hz = ROM_SysCtlClockFreqSet(
                               (SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN |
                                SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480),
                               SYSTEM_CLOCK);
    ASSERT(output_clock_rate_hz == SYSTEM_CLOCK);

    // Initialize the GPIO pins for the Launchpad
    PinoutSet(false, false);

    // Set up the UART which is connected to the virtual COM port
    UARTStdioConfig(0, 57600, SYSTEM_CLOCK);

    queue_init();

    // Create led task
    xTaskCreate(LEDTask, (const portCHAR *)"LEDs",
                configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    // Create logger task
    xTaskCreate(LogTask, (const portCHAR *)"Log",
                    configMINIMAL_STACK_SIZE, NULL, 1, NULL);


    // Create temp task
    xTaskCreate(TempTask, (const portCHAR *)"Temp",
                       configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    // Create alert task
    xTaskCreate(AlertTask, (const portCHAR *)"alert",
                           configMINIMAL_STACK_SIZE, NULL, 1, &handle);


    /*start the schedule*/
    vTaskStartScheduler();
    return 0;
}


/**********************************************
 *        LED Thread
 **********************************************/
void LEDTask(void *pvParameters)
{
    UARTprintf("Created LED Task\n");

    long x_led_id = LED_ID;
    xTimerHandle xTimer_LED;
    xTimer_LED = xTimerCreate("Timer_LED",              // Just a text name, not used by the kernel.
                              pdMS_TO_TICKS( LED_TOGGLE_PERIOD_MS ),     // 100ms
                              pdTRUE,                   // The timers will auto-reload themselves when they expire.
                              ( void * ) x_led_id,      // Assign each timer a unique id equal to its array index.
                              vTimerCallback_LED_handler// Each timer calls the same callback when it expires.
                             );

     if( xTimer_LED == NULL )
     {
        // The timer was not created.
        UARTprintf("Error on timer creation\n");
     }
     else
     {
        /*Start led timer*/
        xTimerStart( xTimer_LED, 0 );

        for (;;)
        {
            if(FLAG_LED == pdTRUE)
            {
                uint32_t LEDValue;

                /*incrementing toggle count*/
                log_led.count++;


                strcpy(log_led.name,"steve");

                /*Reading the status of the LED Bank*/
                LEDRead(&LEDValue);

                /*LED 1*/
                if((LEDValue & LED_1_ON) == 0x00)
                {
                    // Turn on LED 1
                    GPIOPinWrite(CLP_D1_PORT, CLP_D1_PIN, CLP_D1_PIN);
                }
                else
                {
                    // Turn off LED 1
                    GPIOPinWrite(CLP_D1_PORT, CLP_D1_PIN, 0);
                }

                /*LED 2*/
                if((LEDValue & LED_2_ON) == 0x00)
                {
                    // Turn on LED 2
                    GPIOPinWrite(CLP_D2_PORT, CLP_D2_PIN, CLP_D2_PIN);
                }
                else
                {
                    // Turn off LED 2
                    GPIOPinWrite(CLP_D2_PORT, CLP_D2_PIN, 0);
                }

                /*Time stamp*/
                sprintf(log_led.time_stamp,"%d", xTaskGetTickCount());

                /*Logger*/
                xQueueSendToBack( myQueue_led,( void * ) &log_led, QUEUE_TIMEOUT_TICKS ) ;

                FLAG_LED = pdFALSE;

            }
        }
     }
}

/**********************************************
 *        LED timer handler
 **********************************************/
void vTimerCallback_LED_handler( TimerHandle_t  *pxTimer )
{
   FLAG_LED = pdTRUE;
}


/*  ASSERT() Error function
 *
 *  failed ASSERTS() from driverlib/debug.h are executed in this function
 */
void __error__(char *pcFilename, uint32_t ui32Line)
{
    // Place a breakpoint here to capture errors until logging routine is finished
    while (1)
    {
    }
}
