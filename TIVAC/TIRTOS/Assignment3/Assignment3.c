/*
 * Copyright (c) 2015, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== empty_min.c ========
 */
/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Log.h>                //needed for any Log_info() call

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

/* TI-RTOS Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/PWM.h>
// #include <ti/drivers/I2C.h>
// #include <ti/drivers/SDSPI.h>
// #include <ti/drivers/SPI.h>
 #include <ti/drivers/UART.h>
// #include <ti/drivers/Watchdog.h>
// #include <ti/drivers/WiFi.h>

/* Board Header file */
#include "Board.h"


//------------------------------------------
// TivaWare Header Files
//------------------------------------------
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "driverlib/adc.h"

#define TASKSTACKSIZE   512
#define TASKSTACKSIZE2  768


Task_Struct task0Struct, task1Struct, task2Struct;
Char task0Stack[TASKSTACKSIZE], task1Stack[TASKSTACKSIZE], task2Stack[TASKSTACKSIZE2];
Task_Handle task;
uint16_t adcValue;
uint16_t duty = 0;
uint8_t counter = 0;
uint32_t ui32ADCValue[1];

UART_Handle uart;
UART_Params uartParams;
char buffer[20];
UART_Handle uart;
UART_Params uartParams;
const char echoPrompt[] = "\fUART Initialized!!\r\n";
const char writeDone[] = "ADC Value: ";
/*
 *  ======== heartBeatFxn ========
 *  Toggle the Board_LED0. The Task_sleep is determined by arg0 which
 *  is configured for the heartBeat Task instance.
 */
Void heartBeatFxn(UArg arg0, UArg arg1)
{
    while (1) {
        Task_sleep((UInt)arg0);
        GPIO_toggle(Board_LED2);    //PF1, RED LED
    }
}

void pwmFxn(UArg arg0, UArg arg1)
{
    PWM_Handle pwm1;
    PWM_Params pwmParams;
    uint16_t pwmPeriod = 4096;      //period and duty in ms

    PWM_Params_init(&pwmParams);
    pwmParams.period = pwmPeriod;
    pwm1 = PWM_open(Board_PWM0, &pwmParams);
    if(pwm1 == NULL)
    {
        System_abort("Board_PWM0 did not open");
    }

    //updating the duty cycle.
    while(1)
    {
     PWM_setDuty(pwm1, duty);
     Task_sleep((UInt) arg0);
    }
}

void buttonFxn(unsigned int index)
{
    //callback function to the button 0 interrupt
        duty = adcValue;
}

Void echoFxn(UArg arg0, UArg arg1)
{

    /* Create a UART with data processing off. */
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = 9600;
    uart = UART_open(Board_UART0, &uartParams);

    if (uart == NULL) {
        System_abort("Error opening the UART");
    }

    UART_write(uart, echoPrompt, sizeof(echoPrompt));

    /* Loop forever echoing */
    while(1){
        sprintf(buffer, "%i\r\n", adcValue);
        UART_write(uart, writeDone, sizeof(writeDone));
        UART_write(uart, buffer, sizeof(buffer));
    }
}

void hardware_init(void)
{
    uint32_t ui32Period;

    //Set CPU Clock to 40MHz. 400MHz PLL/2 = 200 DIV 5 = 40MHz
    //SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

    // Timer 2 setup code
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);           // enable Timer 2 periph clks
    TimerConfigure(TIMER2_BASE, TIMER_CFG_PERIODIC);        // cfg Timer 2 mode - periodic

    ui32Period = (SysCtlClockGet() /1000);                     // period = CPU clk div 1000 (1ms)
    TimerLoadSet(TIMER2_BASE, TIMER_A, ui32Period);         // set Timer 2 period

    TimerIntEnable(TIMER2_BASE, TIMER_TIMA_TIMEOUT);        // enables Timer 2 to interrupt CPU

    TimerEnable(TIMER2_BASE, TIMER_A);                      // enable Timer 2

    //set up ADC
    //using the A11.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_5);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    ADCHardwareOversampleConfigure(ADC0_BASE, 32);

    ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0); // Changed to sequencer #2

    ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH11|ADC_CTL_IE|ADC_CTL_END);

    ADCIntEnable(ADC0_BASE, 0);
    ADCSequenceEnable(ADC0_BASE, 0);

}
/*
 *  ======== main ========
 */
int main(void)
{
    Task_Params taskParams;
    Task_Params pwmTask;
    Task_Params uartTask;
    /* Call board init functions */
    Board_initGeneral();
    Board_initGPIO();
    Board_initPWM();
    hardware_init();
    // Board_initI2C();
    // Board_initSDSPI();
    // Board_initSPI();
    Board_initUART();
    // Board_initUSB(Board_USBDEVICE);
    // Board_initWatchdog();
    // Board_initWiFi();

    Task_Params_init(&taskParams);
    taskParams.arg0 = 500;
    taskParams.stackSize = TASKSTACKSIZE;
    taskParams.stack = &task0Stack;
    Task_construct(&task0Struct, (Task_FuncPtr)heartBeatFxn, &taskParams, NULL);


    Task_Params_init(&pwmTask);
    pwmTask.arg0 = 50;
    pwmTask.stackSize = TASKSTACKSIZE;
    pwmTask.stack = &task1Stack;
    Task_construct(&task1Struct, (Task_FuncPtr)pwmFxn, &pwmTask, NULL);


    task = Task_handle(&task1Struct);

    Task_Params_init(&uartTask);
    uartTask.stackSize = TASKSTACKSIZE2;
    uartTask.stack = &task2Stack;
    uartTask.instance->name = "echo";
    Task_construct(&task2Struct, (Task_FuncPtr)echoFxn, &uartTask, NULL);
    /* Turn on user LED  */
   // GPIO_write(Board_LED2, Board_LED_ON);
    /* Start BIOS */
    BIOS_start();

    return (0);
}

void timer2_ISR(voide)
{
    TimerIntClear(TIMER2_BASE, TIMER_TIMA_TIMEOUT);         // must clear timer flag FROM timer
    ADCIntClear(ADC0_BASE, 0);
    ADCProcessorTrigger(ADC0_BASE, 0);
    counter++;
    if(counter == 5)
    {
        //perform ADC read
        ADCSequenceDataGet(ADC0_BASE, 0, ui32ADCValue);
        adcValue = ui32ADCValue[0];
    }

    if(counter == 10)
    {
        //print ADC value to the UART
        //
    }

    if(counter ==15)
    {
        //read the value of the button 0. and print the status to the console.
        if(GPIO_read(Board_BUTTON0) == 1)
        {
            System_printf("Button 0 not pressed\n");
            System_flush();
        }
        else
        {
            System_printf("Button 0 is pressed\n");
            System_flush();
        }

        GPIO_setCallback(Board_BUTTON0, buttonFxn);
        GPIO_enableInt(Board_BUTTON0);
        //reset the counter.
        counter = 0;
    }
}
