/*
 * ultraSonic.c
 *
 *  Created on: Nov 20, 2018
 *      Author: cordesM and KoellerN
 */

//**********************************************************************************
// Name(s): Nicklas Koeller && Mason Cordes
// Date: October 5, 2018
// Professor: Dr. Krug
// Description: This program is testing LCD for final Project Trying to get a main menu with highlighting different options
//**********************************************************************************
#include "driverlib.h"
#include "Images.h"


//setting up config structs
    const Timer_A_CaptureModeConfig captureModeConfig =
    {
    TIMER_A_CAPTURECOMPARE_REGISTER_1,                      // CC Register 1
    TIMER_A_CAPTUREMODE_RISING_AND_FALLING_EDGE,            // Rising Edge and falling
    TIMER_A_CAPTURE_INPUTSELECT_CCIxA,                      // CCIxA Input Select
    TIMER_A_CAPTURE_SYNCHRONOUS,                            // Synchronized Capture
    TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE,                // Enable interrupt
    TIMER_A_OUTPUTMODE_OUTBITVALUE                          // Output bit value
    };

    const Timer_A_ContinuousModeConfig continuousModeConfig =
    {
    TIMER_A_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
    TIMER_A_CLOCKSOURCE_DIVIDER_1,      // SMCLK/1 = 12MHz
    TIMER_A_TAIE_INTERRUPT_DISABLE,     // Disable Timer ISR
    TIMER_A_SKIP_CLEAR                  // Skip Clear Counter
    };

/******************************
Name:         timerA_init
Description:  Sets up timerA for ultrasonic sensor
Input:        none
Output:       none
Source(s):    Some of this code was taken from Dr. Krug's Lecture for EGR 326-01 Fall 2018.
*******************************/
void timerA2_init(void)
{
    //Initializations P5.6 Timer A2.1
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P5, GPIO_PIN6, GPIO_PRIMARY_MODULE_FUNCTION);
    /* Configuring Capture Mode */
    MAP_Timer_A_initCapture(TIMER_A2_BASE, &captureModeConfig);
    /* Configuring Continuous Mode */
    MAP_Timer_A_configureContinuousMode(TIMER_A2_BASE,&continuousModeConfig);
    //Enable interrupt
    MAP_Interrupt_enableInterrupt(INT_TA2_N);

}


void ultraSonicPinInit(void)
{
    //Init trigger GPIO pin for ultrasonic
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN1);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN1);
    //LED inits for Onboard LEDs ATM
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN1);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
}


void triggerUltraSonic(void)
{
    //sends 10us pulse to trigger pin
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN1); //turns trig pin on
    systick_delay_us(10);                                 //10 us delay
    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN1); //Turns pin off
}

