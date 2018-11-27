/*
 * ADC.c
 *
 *  Created on: Nov 27, 2018
 *      Author: nicklaskoeller
 */
#include "driverlib.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define BACKLIGHTOFFSET 30

/* Timer_A PWM Configuration Parameter */
Timer_A_PWMConfig pwmConfigBacklight =
{
        TIMER_A_CLOCKSOURCE_SMCLK,
        TIMER_A_CLOCKSOURCE_DIVIDER_1,
        32000,
        TIMER_A_CAPTURECOMPARE_REGISTER_1,
        TIMER_A_OUTPUTMODE_RESET_SET,
        3200
};

void updateBacklight(float normalizedADCRes, float normalizedADCResBat)
{
    static float last = 0;
    int temp = 0;

    if(!ADC14_isBusy ())
            MAP_ADC14_toggleConversionTrigger();

        if((normalizedADCRes >= (last*1.02)) || (normalizedADCRes <= (last *.98)))
                {
                    last = normalizedADCRes;
                    pwmConfigBacklight.dutyCycle = (last*pwmConfigBacklight.timerPeriod + BACKLIGHTOFFSET);
                    MAP_Timer_A_generatePWM(TIMER_A1_BASE, &pwmConfigBacklight);
                }
        temp = 17.7778*normalizedADCResBat;
        driveBatMotor(temp);
}
void ADCBacklightInit(void)
{
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(
            GPIO_PORT_P7, GPIO_PIN7, GPIO_PRIMARY_MODULE_FUNCTION); // Configuring GPIO2.4 as peripheral output for PWM
    MAP_Timer_A_generatePWM(TIMER_A1_BASE, &pwmConfigBacklight);

    /* ADC Init */
    MAP_ADC14_enableModule();
    MAP_ADC14_initModule(ADC_CLOCKSOURCE_SMCLK, ADC_PREDIVIDER_4, ADC_DIVIDER_4,
                         0);

    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(
            GPIO_PORT_P5, GPIO_PIN5|GPIO_PIN4, GPIO_TERTIARY_MODULE_FUNCTION);

    MAP_ADC14_configureMultiSequenceMode(ADC_MEM0, ADC_MEM1, false);
        MAP_ADC14_configureConversionMemory(ADC_MEM0, ADC_VREFPOS_AVCC_VREFNEG_VSS,ADC_INPUT_A0, false);
        MAP_ADC14_configureConversionMemory(ADC_MEM1, ADC_VREFPOS_AVCC_VREFNEG_VSS,ADC_INPUT_A1, false);

    MAP_ADC14_enableSampleTimer(ADC_AUTOMATIC_ITERATION);

    MAP_ADC14_enableConversion();
    MAP_ADC14_toggleConversionTrigger();

    MAP_ADC14_enableInterrupt(ADC_INT1);
    MAP_Interrupt_enableInterrupt(INT_ADC14);

}



