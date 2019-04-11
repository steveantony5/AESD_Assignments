/*
 * temp.c
 *
 *  Created on: Apr 9, 2019
 *      Author: Steve Antony
 */

/**********************************************
 *        Includes
 **********************************************/
#include "temp.h"

/**********************************************
 *        Globals
 **********************************************/
int FLAG_TEMP = 0;
struct log_struct_temp log_temp;

/**********************************************
 *         Temperature thread
 **********************************************/
void TempTask(void *pvParameters)
{
    UARTprintf("Created Temperature Task\n");

    long x_temp_id = 1002;
    xTimerHandle xTimer_Temp;
    xTimer_Temp = xTimerCreate("Timer_Temp",               // Just a text name, not used by the kernel.
                                pdMS_TO_TICKS( TEMP_TIME_PERIOD_MS ),     // 1000ms
                                pdTRUE,                    // The timers will auto-reload themselves when they expire.
                                ( void * ) x_temp_id,      // Assign each timer a unique id equal to its array index.
                                vTimerCallback_Temp_handler// Each timer calls the same callback when it expires.
                               );

    if( xTimer_Temp == NULL )
    {
        // The timer was not created.
        UARTprintf("Error on timer creation - xTimer_Temp\n");
    }

    else
    {
        /*start the timer*/
         xTimerStart( xTimer_Temp, 0 );

         //setup i2c
         i2c_setup();

         for(;;)
         {
             if(FLAG_TEMP == pdTRUE)
             {
                 int16_t temp_value =  read_temp(TEMP_SENSOR_ADDR, TEMP_REG_OFFSET_ADDR);

                 /*convert temperature to celcius*/
                 float temperature_C;
                 temperature_C = temp_value * 0.0625;

                 int32_t temp_dec = (uint32_t)(10000.0 * temperature_C);

                 sprintf(log_temp.time_stamp,"%d", xTaskGetTickCount());
                 sprintf(log_temp.temp,"Temp = %d.%d C", (int32_t)temp_dec/10000, (int32_t)temp_dec%10000);

                 //UARTprintf("%s\n",log_temp.temp);
                 xQueueSendToBack( myQueue_temp,( void * ) &log_temp, QUEUE_TIMEOUT_TICKS ) ;

                 /*Notification*/
                 if((temperature_C > TEMP_THRESHOLD_HIGHER) || (temperature_C < TEMP_THRESHOLD_LOWER))
                 {
                     /*notify*/
                     xTaskNotifyGive(handle);
                 }

                 FLAG_TEMP = pdFALSE;

             }
         }

    }
}


/**********************************************
 *         Alert thread
 **********************************************/
void AlertTask(void *pvParameters)
{
    for(;;)
    {
        uint32_t ulNotifiedValue = 0;

        ulNotifiedValue  = ulTaskNotifyTake( pdTRUE, NOTIFY_TAKE_TIMEOUT  );

        if(ulNotifiedValue > 0)
        {
            /*log the notification*/
            xQueueSendToBack( myQueue_alert,( void * ) &ulNotifiedValue, QUEUE_TIMEOUT_TICKS ) ;
        }
    }

}

/**********************************************
 *         Temp timer handler
 **********************************************/
void vTimerCallback_Temp_handler( TimerHandle_t  *pxTimer )
{
    FLAG_TEMP = pdTRUE;
}


void i2c_setup()
{
    /*Enabling i2c pheripheral*/
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C2);

    /*Enabling GPIO*/
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);

    /*Configuring I2C SDA GPIO*/
    GPIOPinConfigure(GPIO_PN4_I2C2SDA);

    /*Configuring I2C SCL GPIO*/
    GPIOPinConfigure(GPIO_PN5_I2C2SCL);



    /*Configuring ic2 SCL*/
    GPIOPinTypeI2CSCL(GPIO_PORTN_BASE, GPIO_PIN_5);

    /*Configuring ic2 SDA*/
    GPIOPinTypeI2C(GPIO_PORTN_BASE, GPIO_PIN_4);

    /*wait till specified peripheral is ready
     * returns true if ready*/
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_I2C2));

    /*initiating i2c master*/
    I2CMasterInitExpClk(I2C2_BASE, output_clock_rate_hz, false);
}

int16_t read_temp(uint8_t temp_sensor_address, uint8_t register_offset)
{

    /*Sets the address that the I2C Master places on the bus*/
    I2CMasterSlaveAddrSet(I2C2_BASE, temp_sensor_address, false);

    /*Transmits a byte from the I2C Master*/
    I2CMasterDataPut(I2C2_BASE, register_offset);

    /*Controls the state of the I2C Master*/
    I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_SINGLE_SEND);


    /*Wait until master says it is busy*/
    while(!I2CMasterBusy(I2C2_BASE));


    /*Indicates whether I2C Master is busy
     * Returns true if the I2C Master is busy*/
     while(I2CMasterBusy(I2C2_BASE));

     /*Sets the address that the I2C Master places on the bus*/
    I2CMasterSlaveAddrSet(I2C2_BASE, temp_sensor_address, true);

    /*Controls the state of the I2C Master*/
    I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);

    /*Wait until master says it is busy*/
     while(!I2CMasterBusy(I2C2_BASE));

    /*Indicates whether I2C Master is busy
     * Returns true if the I2C Master is busy*/
    while(I2CMasterBusy(I2C2_BASE));

    /*Get the MS Byte*/
    uint8_t higherByte = I2CMasterDataGet(I2C2_BASE);

    I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);

    /*Wait until master says it is busy*/
    while(!I2CMasterBusy(I2C2_BASE));


    /*Indicates whether I2C Master is busy
     * Returns true if the I2C Master is busy*/
    while(I2CMasterBusy(I2C2_BASE));

    /*Get the LS Byte*/
    uint8_t lowerByte = I2CMasterDataGet(I2C2_BASE);

    /*Formatting the data*/
    int16_t value;

    value = ((higherByte << 8) | lowerByte) >> 4;

    /*Checks if the value is negative
     * Check the 12th bit*/
    if((value & 0x800) == 0x800)
    {
        value = ~(value);
        return ((value+1)*(-1));
    }
    else
    {
        /*returns the temperature*/
        return value;
    }

}
