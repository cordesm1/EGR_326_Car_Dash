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
     12000000, // SMCLK = 12MHz
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
    //just set random values while testing the write
    MAP_I2C_setMode(EUSCI_B2_BASE, EUSCI_B_I2C_TRANSMIT_MODE);  //Set Master in transmit mode

    while (MAP_I2C_isBusBusy(EUSCI_B2_BASE));   // Wait for bus release, ready to write

    MAP_I2C_masterSendMultiByteStart(EUSCI_B2_BASE,0); // set pointer to beginning of RTC registers

    MAP_I2C_masterSendMultiByteNext(EUSCI_B2_BASE, 0x00);   // and write to seconds register

    MAP_I2C_masterSendMultiByteNext(EUSCI_B2_BASE, 0x49); // write to minutes register

    MAP_I2C_masterSendMultiByteNext(EUSCI_B2_BASE, 0x10); // write to hours register

    MAP_I2C_masterSendMultiByteNext(EUSCI_B2_BASE, 0x04); // write to day register

    MAP_I2C_masterSendMultiByteNext(EUSCI_B2_BASE, 0x14); // write to date register

    MAP_I2C_masterSendMultiByteNext(EUSCI_B2_BASE, 0x11); // write to months register

    MAP_I2C_masterSendMultiByteFinish(EUSCI_B2_BASE, 0x18); // write to year register and send stop

    //readI2C(1); //Reads the time from the RTC to fix bug with first read being garbage
}

/******************************
 Name:         readI2C
 Description:  Reads data from the I2C bus
 Input:        none
 Output:       none
 Source(s):    Some of this code is taken from Dr. Krug's Lecture 7 for EGR326-10 Fall 2018
 *******************************/
//void readI2C(uint8_t mode)
//{
//    // Set Master in transmit mode
//    MAP_I2C_setMode(EUSCI_B2_BASE, EUSCI_B_I2C_TRANSMIT_MODE);
//    // Wait for bus release, ready to write
//    while (MAP_I2C_isBusBusy(EUSCI_B2_BASE));
//    // set pointer to beginning of RTC registers
//    if(mode == 1)MAP_I2C_masterSendSingleByte(EUSCI_B2_BASE,0);
//    if(mode == 2)MAP_I2C_masterSendSingleByte(EUSCI_B2_BASE,3);
//    if(mode == 3)MAP_I2C_masterSendSingleByte(EUSCI_B2_BASE,17);
//    // Wait for bus release
//    while (MAP_I2C_isBusBusy(EUSCI_B2_BASE));
//    // Set Master in receive mode
//    MAP_I2C_setMode(EUSCI_B2_BASE, EUSCI_B_I2C_RECEIVE_MODE);
//    // Wait for bus release, ready to receive
//    while (MAP_I2C_isBusBusy(EUSCI_B2_BASE));
//    // read from RTC registers (pointer auto increments after each read)
//    if(mode == 1)
//    {
//        read.Second=MAP_I2C_masterReceiveSingleByte(EUSCI_B2_BASE);
//        read.Minute=MAP_I2C_masterReceiveSingleByte(EUSCI_B2_BASE);
//        read.Hour=MAP_I2C_masterReceiveSingleByte(EUSCI_B2_BASE);
//    }
//    if(mode == 2)
//    {
//        read.DayOfWeek=MAP_I2C_masterReceiveSingleByte(EUSCI_B2_BASE);
//        read.Date=MAP_I2C_masterReceiveSingleByte(EUSCI_B2_BASE);
//        read.Month=MAP_I2C_masterReceiveSingleByte(EUSCI_B2_BASE);
//        read.Year=MAP_I2C_masterReceiveSingleByte(EUSCI_B2_BASE);
//    }
//    if(mode == 3)
//    {
//        read.Temp=MAP_I2C_masterReceiveSingleByte(EUSCI_B2_BASE);
//        read.Temp = read.Temp + ((MAP_I2C_masterReceiveSingleByte(EUSCI_B2_BASE) >> 6)*0.25);
//    }
//}

/******************************
 Name:         readTimeIn
 Description:  Reads data from the I2C bus
 Input:        timeArray = array that holds values for time in [1]minutes and [2]hours
 Output:       none
 Source(s):    Some of this code is taken from Dr. Krug's Lecture 7 for EGR326-10 Fall 2018
 *******************************/
void readTimeIn(uint8_t *timeArray)
{

    MAP_Interrupt_disableMaster();           //stops interrupts

    // Set Master in transmit mode
    MAP_I2C_setMode(EUSCI_B2_BASE, EUSCI_B_I2C_TRANSMIT_MODE);
    // Wait for bus release, ready to write
    while (MAP_I2C_isBusBusy(EUSCI_B2_BASE));
    // set pointer to minutes
    MAP_I2C_masterSendSingleByte(EUSCI_B2_BASE,1);
    // Wait for bus release
    while (MAP_I2C_isBusBusy(EUSCI_B2_BASE));
    // Set Master in receive mode
    MAP_I2C_setMode(EUSCI_B2_BASE, EUSCI_B_I2C_RECEIVE_MODE);
    // Wait for bus release, ready to receive
    while (MAP_I2C_isBusBusy(EUSCI_B2_BASE));
    // read from RTC registers (pointer auto increments after each read)
    timeArray[1] = MAP_I2C_masterReceiveSingleByte(EUSCI_B2_BASE);//minutes
    timeArray[2] = MAP_I2C_masterReceiveSingleByte(EUSCI_B2_BASE);//hours

    MAP_Interrupt_enableMaster();           //enables interrupts
}

float readRTCtemp(float RTCtemp)
{
    MAP_Interrupt_disableMaster();           //stops interrupts

    // Set Master in transmit mode
    MAP_I2C_setMode(EUSCI_B2_BASE, EUSCI_B_I2C_TRANSMIT_MODE);
    // Wait for bus release, ready to write
    while (MAP_I2C_isBusBusy(EUSCI_B2_BASE));
    // set pointer to minutes
    MAP_I2C_masterSendSingleByte(EUSCI_B2_BASE,17);
    // Wait for bus release
    while (MAP_I2C_isBusBusy(EUSCI_B2_BASE));
    // Set Master in receive mode
    MAP_I2C_setMode(EUSCI_B2_BASE, EUSCI_B_I2C_RECEIVE_MODE);
    // Wait for bus release, ready to receive
    while (MAP_I2C_isBusBusy(EUSCI_B2_BASE));

    RTCtemp = MAP_I2C_masterReceiveSingleByte(EUSCI_B2_BASE);
    RTCtemp = RTCtemp + ((MAP_I2C_masterReceiveSingleByte(EUSCI_B2_BASE) >> 6)*0.25);

    MAP_Interrupt_enableMaster();           //enables interrupts
    return(RTCtemp);
}


/******************************
 Name:         initRTC
 Description:  Initalizes the RTC module on uesci_B0
 Input:        none
 Output:       none
 Source(s):    Some of this code is taken from Dr. Krug's Lecture 7 for EGR326-10 Fall 2018
 *******************************/
void initRTC(void)
{
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P3, GPIO_PIN6 + GPIO_PIN7, GPIO_PRIMARY_MODULE_FUNCTION); //Setting P3.6 for SDA, and P3.7 for SCL
    MAP_I2C_initMaster(EUSCI_B2_BASE,  &i2cConfig); //Initialize MSP as Master with parameters of i2cConfig
    MAP_I2C_setSlaveAddress(EUSCI_B2_BASE,  0b1101000); //Specifies the slave address
    MAP_I2C_setMode (EUSCI_B2_BASE, EUSCI_B_I2C_TRANSMIT_MODE); //Sets the MSP to transmit mode
    MAP_I2C_enableModule(EUSCI_B2_BASE); //Starts the I2C Module Communication
}

/******************************
 Name:         readFullRTC
 Description:  reads in all time registers from RTC
 Input:        timeArray =  holds all time and date vars for RTC
 Output:       none
 Source(s):    Some of this code is taken from Dr. Krug's Lecture 7 for EGR326-10 Fall 2018
 *******************************/
void readFullRTC(uint8_t *timeArray)
{
    int i = 0;

    MAP_Interrupt_disableMaster();           //stops interrupts

    // Set Master in transmit mode
    MAP_I2C_setMode(EUSCI_B2_BASE, EUSCI_B_I2C_TRANSMIT_MODE);
    // Wait for bus release, ready to write
    while (MAP_I2C_isBusBusy(EUSCI_B2_BASE));
    // set pointer to minutes
    MAP_I2C_masterSendSingleByte(EUSCI_B2_BASE,0);
    // Wait for bus release
    while (MAP_I2C_isBusBusy(EUSCI_B2_BASE));
    // Set Master in receive mode
    MAP_I2C_setMode(EUSCI_B2_BASE, EUSCI_B_I2C_RECEIVE_MODE);
    // Wait for bus release, ready to receive
    while (MAP_I2C_isBusBusy(EUSCI_B2_BASE));
    // read from RTC registers (pointer auto increments after each read)
    for(i = 0; i<7; i++)
        timeArray[i] = MAP_I2C_masterReceiveSingleByte(EUSCI_B2_BASE); //writes seconds to year in the RTC

    MAP_Interrupt_enableMaster();           //enable interrupts
}


void writeFullRTC(uint8_t *timeArray)
{
    MAP_Interrupt_disableMaster();           //stops interrupts

    uint8_t garbageRead[7];
    //just set random values while testing the write
    MAP_I2C_setMode(EUSCI_B2_BASE, EUSCI_B_I2C_TRANSMIT_MODE);  //Set Master in transmit mode

    while (MAP_I2C_isBusBusy(EUSCI_B2_BASE));   // Wait for bus release, ready to write

    MAP_I2C_masterSendMultiByteStart(EUSCI_B2_BASE,0); // set pointer to beginning of RTC registers

    MAP_I2C_masterSendMultiByteNext(EUSCI_B2_BASE,   timeArray[0]);   // and write to seconds register

    MAP_I2C_masterSendMultiByteNext(EUSCI_B2_BASE,   timeArray[1]); // write to minutes register

    MAP_I2C_masterSendMultiByteNext(EUSCI_B2_BASE,   timeArray[2]); // write to hours register

    MAP_I2C_masterSendMultiByteNext(EUSCI_B2_BASE,   timeArray[3]); // write to day register

    MAP_I2C_masterSendMultiByteNext(EUSCI_B2_BASE,   timeArray[4]); // write to date register

    MAP_I2C_masterSendMultiByteNext(EUSCI_B2_BASE,   timeArray[5]); // write to months register

    MAP_I2C_masterSendMultiByteFinish(EUSCI_B2_BASE, timeArray[6]); // write to year register and send stop

    readFullRTC(garbageRead);

    MAP_Interrupt_enableMaster();           //enable interrupts
}


void writeTimeOnly(uint8_t *timeArray)
{
    uint8_t hours, mins;
    mins = timeArray[1];
    hours = timeArray[2];

    MAP_Interrupt_disableMaster();           //stops interrupts

    MAP_I2C_setMode(EUSCI_B2_BASE, EUSCI_B_I2C_TRANSMIT_MODE);  //Set Master in transmit mode

    while (MAP_I2C_isBusBusy(EUSCI_B2_BASE));   // Wait for bus release, ready to write

    MAP_I2C_masterSendMultiByteStart(EUSCI_B2_BASE,1); // set pointer to beginning of RTC registers

    MAP_I2C_masterSendMultiByteNext(EUSCI_B2_BASE, mins);   // write to minutes register

    MAP_I2C_masterSendMultiByteFinish(EUSCI_B2_BASE, hours); // write to year register and send stop

    MAP_Interrupt_enableMaster();           //enable interrupts
}



void writeDateOnly(uint8_t *timeArray)
{

    MAP_Interrupt_disableMaster();           //stops interrupts
    uint8_t date,month,year;
    date = timeArray[4];
    month = timeArray[5];
    year = timeArray[6];

    MAP_I2C_setMode(EUSCI_B2_BASE, EUSCI_B_I2C_TRANSMIT_MODE);  //Set Master in transmit mode

    while (MAP_I2C_isBusBusy(EUSCI_B2_BASE));   // Wait for bus release, ready to write

    MAP_I2C_masterSendMultiByteStart(EUSCI_B2_BASE,4); // set pointer to beginning of RTC registers

    MAP_I2C_masterSendMultiByteNext(EUSCI_B2_BASE, date);   // date

    MAP_I2C_masterSendMultiByteNext(EUSCI_B2_BASE, month);   // month

    MAP_I2C_masterSendMultiByteFinish(EUSCI_B2_BASE, year);        // year

    MAP_Interrupt_enableMaster();           //enable interrupts
}

