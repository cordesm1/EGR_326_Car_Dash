/*
 * RTC.c
 *
 *  Created on: Nov 13, 2018
 *      Author: nicklaskoeller
 */
#include "driverlib.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "RTC.h"

const eUSCI_I2C_MasterConfig i2cConfig =
{
     EUSCI_B_I2C_CLOCKSOURCE_SMCLK, // SMCLK Clock Source
     3000000, // SMCLK = 3MHz
     EUSCI_B_I2C_SET_DATA_RATE_100KBPS, // Desired I2C Clock of 100khz
     0, // No byte counter threshold
     EUSCI_B_I2C_NO_AUTO_STOP // No Autostop
};

/******************************
 Name:         writeI2C
 Description:  Writes to memory adresses on the RTC module
 Input:        none
 Output:       none
 Source(s):    Some of this code is taken from Dr. Krug's Lecture 7 for EGR326-10 Fall 2018
 *******************************/
void writeI2C(void)
{

    MAP_I2C_setMode(EUSCI_B0_BASE, EUSCI_B_I2C_TRANSMIT_MODE);  //Set Master in transmit mode

    while (MAP_I2C_isBusBusy(EUSCI_B0_BASE));   // Wait for bus release, ready to write

    MAP_I2C_masterSendMultiByteStart(EUSCI_B0_BASE,0); // set pointer to beginning of RTC registers

    MAP_I2C_masterSendMultiByteNext(EUSCI_B0_BASE, initialize.Second);   // and write to seconds register

    MAP_I2C_masterSendMultiByteNext(EUSCI_B0_BASE, initialize.Minute); // write to minutes register

    MAP_I2C_masterSendMultiByteNext(EUSCI_B0_BASE, initialize.Hour); // write to hours register

    MAP_I2C_masterSendMultiByteNext(EUSCI_B0_BASE, initialize.DayOfWeek); // write to day register

    MAP_I2C_masterSendMultiByteNext(EUSCI_B0_BASE, initialize.Date); // write to date register

    MAP_I2C_masterSendMultiByteNext(EUSCI_B0_BASE, initialize.Month); // write to months register

    MAP_I2C_masterSendMultiByteFinish(EUSCI_B0_BASE, initialize.Year); // write to year register and send stop

    readI2C(1); //Reads the time from the RTC to fix bug with first read being garbage
}

/******************************
 Name:         readI2C
 Description:  Reads data from the I2C bus
 Input:        none
 Output:       none
 Source(s):    Some of this code is taken from Dr. Krug's Lecture 7 for EGR326-10 Fall 2018
 *******************************/
void readI2C(uint8_t mode)
{
    // Set Master in transmit mode
    MAP_I2C_setMode(EUSCI_B0_BASE, EUSCI_B_I2C_TRANSMIT_MODE);
    // Wait for bus release, ready to write
    while (MAP_I2C_isBusBusy(EUSCI_B0_BASE));
    // set pointer to beginning of RTC registers
    if(mode == 1)MAP_I2C_masterSendSingleByte(EUSCI_B0_BASE,0);
    if(mode == 2)MAP_I2C_masterSendSingleByte(EUSCI_B0_BASE,3);
    if(mode == 3)MAP_I2C_masterSendSingleByte(EUSCI_B0_BASE,17);
    // Wait for bus release
    while (MAP_I2C_isBusBusy(EUSCI_B0_BASE));
    // Set Master in receive mode
    MAP_I2C_setMode(EUSCI_B0_BASE, EUSCI_B_I2C_RECEIVE_MODE);
    // Wait for bus release, ready to receive
    while (MAP_I2C_isBusBusy(EUSCI_B0_BASE));
    // read from RTC registers (pointer auto increments after each read)
    if(mode == 1)
    {
        read.Second=MAP_I2C_masterReceiveSingleByte(EUSCI_B0_BASE);
        read.Minute=MAP_I2C_masterReceiveSingleByte(EUSCI_B0_BASE);
        read.Hour=MAP_I2C_masterReceiveSingleByte(EUSCI_B0_BASE);
    }
    if(mode == 2)
    {
        read.DayOfWeek=MAP_I2C_masterReceiveSingleByte(EUSCI_B0_BASE);
        read.Date=MAP_I2C_masterReceiveSingleByte(EUSCI_B0_BASE);
        read.Month=MAP_I2C_masterReceiveSingleByte(EUSCI_B0_BASE);
        read.Year=MAP_I2C_masterReceiveSingleByte(EUSCI_B0_BASE);
    }
    if(mode == 3)
    {
        read.Temp=MAP_I2C_masterReceiveSingleByte(EUSCI_B0_BASE);
        read.Temp = read.Temp + ((MAP_I2C_masterReceiveSingleByte(EUSCI_B0_BASE) >> 6)*0.25);

    }
}



