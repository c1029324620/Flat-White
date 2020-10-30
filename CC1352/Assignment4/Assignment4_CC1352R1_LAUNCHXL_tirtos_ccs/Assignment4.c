/*
 * Copyright (c) 2015-2019, Texas Instruments Incorporated
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
 *  ======== hello.c ========
 */

/* XDC Module Headers */
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/cfg/global.h>


/* BIOS Module Headers */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Semaphore.h>


#include <ti/drivers/Board.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/ADC.h>
#include <ti/display/Display.h>
#include <ti/drivers/PWM.h>
#include <ti/drivers/Timer.h>
  /* Driver configuration */
#include "ti_drivers_config.h"

uint16_t timerCNT = 0;
uint16_t adcValue;
ADC_Handle adc;
static Display_Handle display;
uint16_t pwmPeriod = 4096;
uint16_t duty = 0;
PWM_Handle pwm1 = NULL;

void timerCallback(Timer_Handle myHandle, int_fast16_t status)
{
    timerCNT++;
    if(timerCNT == 5)
    {
        Semaphore_post(semHandle);
    }
    if(timerCNT == 10)
    {
        Semaphore_post(semHandle1);
    }
    if(timerCNT == 15)
    {
        Semaphore_post(semHandle2);
        timerCNT = 0;
    }
}

void heartBeatFxN(UArg arg0, UArg arg1)
{
    while(1)
    {
        GPIO_write(CONFIG_GPIO_LED_0, CONFIG_LED_OFF);
        Task_sleep(500*(1000/Clock_tickPeriod));
        GPIO_write(CONFIG_GPIO_LED_0, CONFIG_LED_ON);
        Task_sleep(500*(1000/Clock_tickPeriod));
    }
}

void adcFxN(UArg arg0, UArg arg1)
{
    while(1){
        Semaphore_pend(semHandle, BIOS_WAIT_FOREVER);
        ADC_convert(adc, &adcValue);
        //Semaphore_post(semHandle);
        Task_sleep(500*(1000/Clock_tickPeriod));
    }
}

void displayFxN(UArg arg0, UArg arg1)
{
    while(1){
        Semaphore_pend(semHandle1, BIOS_WAIT_FOREVER);
        Display_printf(display, 1, 0, "ADC Value: %d\n", adcValue);
       // Semaphore_post(semHandle1);
        Task_sleep(500*(1000/Clock_tickPeriod));
    }
}

void pwmFxN(UArg arg0, UArg arg1)
{
    while(1){
        Semaphore_pend(semHandle2, BIOS_WAIT_FOREVER);
        duty = adcValue;
        PWM_setDuty(pwm1, duty);
        Display_printf(display, 1, 0, "PWM duty cycle: %d\n", duty);
        //Semaphore_post(semHandle);
        Task_sleep(500*(1000/Clock_tickPeriod));

    }
}


/*
 *  ======== main ========
 */
int main()
{
    /* Call driver init functions */
    Board_init();
    GPIO_init();
    ADC_init();
    PWM_init();
    Display_init();
    Timer_init();

    Timer_Handle timer0;
    Timer_Params timerParams;

    /*
     * Setting up the timer in continuous callback mode that calls the callback
     * function every 1000 microseconds or 1ms.
     */
    Timer_Params_init(&timerParams);
    timerParams.period = 1000;
    timerParams.periodUnits = Timer_PERIOD_US;
    timerParams.timerMode = Timer_CONTINUOUS_CALLBACK;
    timerParams.timerCallback = timerCallback;

    timer0 = Timer_open(CONFIG_TIMER_0, &timerParams);
    if (timer0 == NULL) {
        // Failed to initialized timer
        Display_printf(display, 0, 0, "Error initializing CONFIG_TIMER_0\n");
        while (1);
    }
    if (Timer_start(timer0) == Timer_STATUS_ERROR) {
        // Failed to start timer
        Display_printf(display, 0, 0, "Error to start timer\n");
        while (1);
    }
    ADC_Params adcParams;
  //  int_fast16_t res;
    ADC_Params_init(&adcParams);
    adc = ADC_open(CONFIG_ADC_0, &adcParams);
    if(adc == NULL) {
        Display_printf(display, 0, 0, "Error initializing CONFIG_ADC_0\n");
        while(1);
    }

    display = Display_open(Display_Type_UART, NULL);
    if (display == NULL) {
        /* Failed to open display driver */
        while (1);
    }
    PWM_Params pwmParams;
    PWM_Params_init(&pwmParams);
    pwmParams.dutyUnits = PWM_DUTY_US;
    pwmParams.dutyValue = 0;
    pwmParams.periodUnits = PWM_PERIOD_US;
    pwmParams.periodValue = pwmPeriod;
    pwm1 = PWM_open(CONFIG_PWM_0, &pwmParams);
    if (pwm1 == NULL) {
        /* CONFIG_PWM_0 did not open */
        Display_printf(display, 0, 0, "Error initializing CONFIG_PWM_0\n");
        while (1);
    }
    PWM_start(pwm1);


    /*
     *  normal BIOS programs, would call BIOS_start() to enable interrupts
     *  and start the scheduler and kick BIOS into gear.  But, this program
     *  is a simple sanity test and calls BIOS_exit() instead.
     */
    BIOS_start();  /* terminates program and dumps SysMin output */
    ADC_close(adc);
    return(0);
}
/*
 *  ======== myDelay ========
 *  Assembly function to delay. Decrements the count until it is zero
 *  The exact duration depends on the processor speed.
 */
__asm("    .sect \".text:myDelay\"\n"
      "    .clink\n"
      "    .thumbfunc myDelay\n"
      "    .thumb\n"
      "    .global myDelay\n"
      "myDelay:\n"
      "    subs r0, #1\n"
      "    bne.n myDelay\n"
      "    bx lr\n");

