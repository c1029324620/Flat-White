#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "driverlib/debug.h"
#include "driverlib/adc.h"
#include "utils/uartstdio.h"
#include <string.h>

#ifdef DEBUG
void__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif


// Globals
uint32_t ui32Period;
char     buffer[4];
uint8_t status = 0x0E;

uint32_t ui32ADC0Value[4];
volatile uint32_t ui32TempAvg;
volatile uint32_t ui32TempValueC;
volatile uint32_t ui32TempValueF;

// Timer 1 ISR
void Timer1IntHandler(void)
{
     // Clear the timer interrupt
     TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

     //ADC keep running.
     ADCIntClear(ADC0_BASE, 2);
     ADCProcessorTrigger(ADC0_BASE, 2);

     ADCSequenceDataGet(ADC0_BASE, 2, ui32ADC0Value);
     ui32TempAvg = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3] + 2)/4;
     ui32TempValueC = (1475 - ((2475 * ui32TempAvg)) / 4096)/10;
     ui32TempValueF = ((ui32TempValueC * 9) + 160) / 5;
}

void display_tem_c()
{
    UARTprintf("Temperature in C: %3d\n", ui32TempValueC);
}

void display_tem_f()
{
    UARTprintf("Temperature in F: %3d\n", ui32TempValueF);
}

void red_on()
{
    //turn RED LED on
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);
}

void blue_on()
{
    //turn BLUE LED on
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);
}

void green_on()
{
    //Turn GREEN LED on
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);
}

void red_off()
{
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x0);
}

void blue_off()
{
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x0);
}

void green_off()
{
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x0);
}

void get_status()
{
    uint8_t red, green, blue;
    red = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_1);
    blue = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2);
    green = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_3);

    if(red == 0x0)
        UARTprintf("Red LED is OFF\n");
    else
        UARTprintf("Red LED is ON\n");
    if(green == 0x0)
        UARTprintf("Green LED is OFF\n");
    else
        UARTprintf("Green LED is ON\n");
    if(blue == 0x0)
        UARTprintf("Blue LED is OFF\n");
    else
        UARTprintf("Blue LED is ON\n");


}

int main(void)
{
    //40MHz
    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);    //Enabling UART
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);    //Enabling GPIO port A
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);  // Enabling Timer 1
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);   //enable GPIOF port.
    // Configure ADC
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    ADCHardwareOversampleConfigure(ADC0_BASE, 32);

    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);

    ADCSequenceConfigure(ADC0_BASE, 2, ADC_TRIGGER_PROCESSOR, 0); // Changed to sequencer #2

    ADCSequenceStepConfigure(ADC0_BASE, 2, 0, ADC_CTL_TS);
    ADCSequenceStepConfigure(ADC0_BASE, 2, 1, ADC_CTL_TS);
    ADCSequenceStepConfigure(ADC0_BASE, 2, 2, ADC_CTL_TS);

    ADCSequenceStepConfigure(ADC0_BASE, 2, 3, ADC_CTL_TS|ADC_CTL_IE|ADC_CTL_END);

    ADCSequenceEnable(ADC0_BASE, 2);

    // Configure Timer 1 module
    TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);
    ui32Period = SysCtlClockGet()/2;   // Period of 0.5s 2Hz
    TimerLoadSet(TIMER1_BASE, TIMER_A, ui32Period -1);
    IntEnable(INT_TIMER1A);
    TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

    // Configure pins for UART
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
    UARTStdioConfig(0, 115200, 16000000);
    // Enable interrupts
    IntMasterEnable();
    TimerEnable(TIMER1_BASE, TIMER_A);
    ADCSequenceEnable(ADC0_BASE, 2);
    UARTprintf("***************Enter the command*****************\n");
    UARTprintf("**R|r => Red LED  B|b => Blue LED G|g => Green LED**\n");
    UARTprintf("***T => Temp in C | t => Temp in F | S => RGB Status***\n");
    while(1)
    {
        switch(UARTCharGet(UART0_BASE))
        {
            case 'T':
                display_tem_c();
                break;
            case 't':
                display_tem_f();
                break;
            case 'R':
                red_on();
                break;
            case 'r':
                red_off();
                break;
            case 'B':
                blue_on();
                break;
            case 'b':
                blue_off();
                break;
            case 'G':
                green_on();
                break;
            case 'g':
                green_off();
                break;
            case 'S':
                get_status();
                break;
            default:
                break;
        }
    }
}

