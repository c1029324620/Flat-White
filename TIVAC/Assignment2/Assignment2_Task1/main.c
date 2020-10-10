#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "utils/uartstdio.h"
#include "driverlib/uart.h"

#include "math.h"
#include "IQmath/IQmathlib.h"

#define TMP006_ADDR (0x40)
#define TMP006_ID   (0xFF)
#define M_ID    (0xFE)
#define TMP006_COMMAND_BIT (0x01)
#define V_OBJ   (0x00)
#define T_DIE   (0x01)
#define TMP006_CONFIG   (0x02)


#define ONE 1.0
#define LSB 1.5625e-7
#define S0 6e-5
#define A1 1.75e-3
#define A2 -1.678e-5
#define B0 -2.94e-5
#define B1 -5.7e-7
#define B2 4.63e-9
#define C2 13.4
#define T_REF 25.13

uint16_t XOR = 0xFFFF;

uint16_t TMP006_Voltage;
uint16_t TMP006_Temp;

double temp;
double TDIE;
double F;
double S;

void ConfigureUART0(void)
//Configures the UART to run at 115200 baud rate
{
    // ENABLE PERIPHERAL UART 0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    // ENABLE GPIO PORT A,FOR UART
    GPIOPinConfigure(GPIO_PA0_U0RX); // PA0 IS CONFIGURED TO UART RX
    GPIOPinConfigure(GPIO_PA1_U0TX); // PA1 IS CONFIGURED TO UART TX
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
    UARTStdioConfig(0, 115200, 16000000);
}

void I2C_Init()
{
    // Enable I2C0 peripheral
  SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
  SysCtlPeripheralReset(SYSCTL_PERIPH_I2C0);

  // Enable GPIO Port B to be used for I2C0
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

  GPIOPinConfigure(GPIO_PB2_I2C0SCL);
  GPIOPinConfigure(GPIO_PB3_I2C0SDA);

  // Configure the pin muxing for I2C0 functions on Port B2 and B3
  GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
  GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);

  I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), false);
  // Set write mode
  UARTprintf("I2C Master communication with Slave Address\n");
  SysCtlDelay(10000);
  UARTprintf("I2C Init complete!\n");
  SysCtlDelay(5000000);
}

void I2C0_Send16(uint8_t slave_addr, uint8_t pointer_reg, uint16_t TxData)
{
  uint8_t data;
  I2CMasterSlaveAddrSet(I2C0_BASE, slave_addr, false);
  I2CMasterDataPut(I2C0_BASE, pointer_reg);
  I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);
  while(I2CMasterBusy(I2C0_BASE));
  // MSB First
  data = (uint8_t)((TxData >> 8) & 0x00FF);
  I2CMasterDataPut(I2C0_BASE, data);
  while(I2CMasterBusy(I2C0_BASE));
  //LSB Later
  data = (uint8_t)(TxData  & 0x00FF);
  I2CMasterDataPut(I2C0_BASE, data);
  I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
  while(I2CMasterBusy(I2C0_BASE));
}

uint16_t I2C_Read(uint8_t slave_addr, uint8_t pointer_reg)
{
  uint8_t UpperByte = 0;
  uint8_t LowerByte = 0;
  uint16_t Data = 0;

  // Specify Slave device address to write to
  I2CMasterSlaveAddrSet(I2C0_BASE, slave_addr, false);

  // Send Register address on Slave device
  I2CMasterDataPut(I2C0_BASE, pointer_reg);

  // Initiate send of register address from Master to Slave
  I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);
  while(I2CMasterBusy(I2C0_BASE));

  // Set read mode
  I2CMasterSlaveAddrSet(I2C0_BASE, slave_addr, true);

  // Get first byte from slave and ack for more
  I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);
  while(I2CMasterBusy(I2C0_BASE));
  UpperByte = I2CMasterDataGet(I2C0_BASE);

  // Get second byte from slave and nack for complete
  I2CMasterControl(I2C0_BASE,  I2C_MASTER_CMD_BURST_RECEIVE_CONT);

  while(I2CMasterBusy(I2C0_BASE));
  LowerByte = I2CMasterDataGet(I2C0_BASE);

  Data = (UpperByte<<8) | LowerByte;
  return Data;
}


int main(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
    ConfigureUART0();
    I2C_Init();
    double IQ_NUM_Mul_Const = pow(2, -24);
    _iq24 v_obj, t_die, s0, a1, a2, t_ref, b0, b1, b2, c2, s, t_die_sub_t_ref, v_os, f_v_obj, v_obj_sub_v_os, one;
    _iq24 lsb;
    s0 = _IQ24(S0);
    a1 = _IQ24(A1);
    a2 = _IQ24(A2);
    t_ref = _IQ24(T_REF);
    b0 = _IQ24(B0);
    b1 = _IQ24(B1);
    b2 = _IQ24(B2);
    c2 = _IQ24(C2);
    lsb = _IQ24(LSB);
    one = _IQ24(ONE);

    while(1)
    {

        TMP006_Voltage = I2C_Read(TMP006_ADDR, V_OBJ);
        if(TMP006_Voltage > 0x7FFF)     //full scale from 0 to 7FFFh, if the reading is negative, get its absolute value.
        {
            //UARTprintf("negative\n");
            TMP006_Voltage = (TMP006_Voltage ^ XOR) + 1;
        }
        //TMP006_Voltage = TMP006_Voltage;
        UARTprintf("I2C V_OBJ: %d\n", TMP006_Voltage);

        TMP006_Temp = (I2C_Read(TMP006_ADDR, T_DIE)>>2)/32 ; //Converting the integer temperature result of the TMP006
                                                              //to physical temperature is done by rightshifting the last two LSBs
        t_die = _IQ24(TMP006_Temp);                             //followed by a divide-by-32 of TREG to obtain the physical temperature result in degreesCelsius. T

        v_obj = _IQ24(TMP006_Voltage);
        v_obj = _IQ24mpy(v_obj, lsb);
        UARTprintf("I2C T_DIE: %d C\n", TMP006_Temp);

        t_die_sub_t_ref = t_die - t_ref;
        s = _IQ24mpy(s0,(_IQ24mpy(a1, t_die_sub_t_ref) + _IQ24mpy(a2, (_IQ24mpy(t_die_sub_t_ref, t_die_sub_t_ref))) + one));
        v_os = _IQ24mpy(t_die_sub_t_ref, b1) + _IQ24mpy(b2,(_IQ24mpy(t_die_sub_t_ref, t_die_sub_t_ref))) + b0;
        v_obj_sub_v_os = v_obj - v_os;
        f_v_obj = v_obj_sub_v_os + _IQ24mpy(c2,_IQ24mpy(v_obj_sub_v_os, v_obj_sub_v_os));

        //Convert to double.
        F = f_v_obj * IQ_NUM_Mul_Const;//convert to double
        S = s * IQ_NUM_Mul_Const;
        TDIE = t_die * IQ_NUM_Mul_Const;
        temp = pow((pow(TDIE, 4.0) + F/S), 0.25);
        SysCtlDelay(5000000);
    }
}
