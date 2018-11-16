/*
 * speed.c
 *
 *  Created on: Nov 16, 2018
 *      Author: cordesM and koellerN
 *  Description:  Holds functions for speedometer and speed related functions
 */




#include <driverlib.h>
#include "speed.h"


Timer_A_PWMConfig pwmConfig =
{
 TIMER_A_CLOCKSOURCE_SMCLK,
 TIMER_A_CLOCKSOURCE_DIVIDER_16,
 18750,
 TIMER_A_CAPTURECOMPARE_REGISTER_1,
 TIMER_A_OUTPUTMODE_RESET_SET,
 0
};

//uint8_t button = 0, config = 0;
//uint32_t speed = 0, count = 0;


void initMotorPWM(void)
{
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN4, GPIO_PRIMARY_MODULE_FUNCTION);
    MAP_Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);                                   //2.4 PWM pin for motor
}


void initTimer32For100us(void)
{
    //Sets time interval of 100us
    MAP_Timer32_initModule(TIMER32_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT,TIMER32_PERIODIC_MODE);

    MAP_Interrupt_enableInterrupt(INT_T32_INT1);
    MAP_Interrupt_enableMaster();

    MAP_Timer32_setCount(TIMER32_BASE,48000000);
    MAP_Timer32_enableInterrupt(TIMER32_BASE);
    MAP_Timer32_startTimer(TIMER32_BASE, true);
}

void initHallEffectPins(void)
{
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P6, GPIO_PIN3); //Sets P1.1 as an input pin with pull up resistor
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P6, GPIO_PIN3); //Sets up Interrupt for P1.1 on falling edge
    MAP_GPIO_enableInterrupt(GPIO_PORT_P6, GPIO_PIN3); //Enables interrupt for P1.1
    MAP_Interrupt_enableInterrupt(INT_PORT6);   //Enables interrupts for port 1
    MAP_GPIO_interruptEdgeSelect(GPIO_PORT_P6, GPIO_PIN3,GPIO_HIGH_TO_LOW_TRANSITION); //Set the interrupt to falling edge
}



void changeMotorPWMspeed(uint8_t motorSpeed)
{

    if(motorSpeed == 0) //set Duty Cycle to 0 for stop
    {
        TIMER_A0-> CCR[1] = 0;
    }
    if(motorSpeed == 0) //set Duty Cycle to 50 for low speed
    {
        TIMER_A0-> CCR[1] = 9375;
    }
    if(motorSpeed == 0) //set Duty Cycle to 75 for mid speed
    {
        TIMER_A0-> CCR[1] = 14062;
    }
    if(motorSpeed == 0) //set Duty Cycle to 100 for top speed
    {
        TIMER_A0-> CCR[1] = 18750;
    }




}



/******************************
 Name:         PORT6IRQ_Handler
 Description:  Handler for the Port 6 Interrupt
 Input:        none
 Output:       none
 Source(s):    Some of this code was taken from Texas Instrument Example Code.
 *******************************/
//void PORT6_IRQHandler(void)
//{
//    uint32_t status;   //Variable to hold the interrupt status
//
//    status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P6);  //Saves the interrupt status to status variable
//    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P6, status);  //Clears the interrupt flag
//
//    if (status & GPIO_PIN2) //If P1.1 caused the interrupt
//    {
//        config++;
//        if(config == 3) config = 0;
//        MAP_Timer_A_generatePWM(TIMER_A0_BASE, pwmConfig);
//    }
//
//    if (status & GPIO_PIN3)
//    {
//        count++;
//    }
//
//}
//
///* Timer32 ISR */
//void T32_INT1_IRQHandler(void)
//{
//    MAP_Timer32_clearInterruptFlag(TIMER32_BASE);
//    speed = hallEffectMagnetCounts;
//    hallEffectMagnetCounts = 0;
//    MAP_Timer32_setCount(TIMER32_BASE,4800000);
//    MAP_Timer32_startTimer(TIMER32_BASE, true);
//
//}

void initSpeedometer(void)
{
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P10, GPIO_PIN1+GPIO_PIN2+GPIO_PIN3+GPIO_PIN4);    //Sets up output pins for the motor
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P10, GPIO_PIN1+GPIO_PIN2+GPIO_PIN3+GPIO_PIN4); //Sets all pins to low to start
    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P10, GPIO_PIN1+GPIO_PIN3);    //Turns on one pin for each coil of the motor
    driveMotor(160);
    driveMotor(0);
}

void driveMotor(int newSpeed)
{
    static int oldSpeed = 0;

    int cycles = newSpeed-oldSpeed;

    oldSpeed = newSpeed;


    if(cycles > 0)
    {

        while(cycles > 0)
            {
            MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P10, GPIO_PIN1);
            MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P10, GPIO_PIN2);
            _delay_cycles(24000);
            MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P10, GPIO_PIN3);
            MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P10, GPIO_PIN4);
            _delay_cycles(24000);
            MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P10, GPIO_PIN2);
            MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P10, GPIO_PIN1);
            _delay_cycles(24000);
            MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P10, GPIO_PIN4);
            MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P10, GPIO_PIN3);
            _delay_cycles(24000);
            cycles--;
            }
    }

    if(cycles < 0)
    {
        cycles = cycles *(-1);
    while(cycles > 0)
        {
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P10, GPIO_PIN3);
    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P10, GPIO_PIN4);
    _delay_cycles(24000);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P10, GPIO_PIN1);
    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P10, GPIO_PIN2);
    _delay_cycles(24000);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P10, GPIO_PIN4);
    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P10, GPIO_PIN3);
    _delay_cycles(24000);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P10, GPIO_PIN2);
    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P10, GPIO_PIN1);
    _delay_cycles(24000);
    cycles--;
        }
    }
}
