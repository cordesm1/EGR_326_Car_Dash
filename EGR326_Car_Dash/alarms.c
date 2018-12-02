/*
 * alarms.c
 *
 *  Created on: Nov 21, 2018
 *      Author: corde
 */

#include "driverlib.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "ST7735.h"



#define CALIBRATION_START 0x000200000


void saveToFlash(uint8_t dataSet[20] , uint8_t alarmType)
{
    //alarmType 1=speed   2=temp

    uint8_t i, alarmLogSize = 200 , messageSize = 20; // index
    uint8_t read_back_data[200]; // array to hold values read back from flash
    //change 35 to the length of one message

    uint8_t* addr_pointer; // pointer to address in flash for reading back values


    //create algorithems for both alarm logs in the same memory bank
    addr_pointer = CALIBRATION_START + 4; // point to address in flash for saving data
    if(alarmType == 1)//This will write a new temp alarm to the first set of messages
    {
       //gets rid of the last temp alarm and shifts them all down one to make room for the new on
       for (i = messageSize; i < messageSize*4; i++)
       { // read values in flash before programming
           read_back_data[i] = *addr_pointer++;
       }
       //keeps second 5 speed alarms in the same place
       for (i = messageSize*4; i<alarmLogSize; i++)
       {
           read_back_data[i] = *addr_pointer++;
       }
       //places the new temp alarm in the first spot
       for (i = 0 ; i < messageSize; i++)
       {
           read_back_data[i] = dataSet[i];
       }

    }

    if(alarmType == 2)//this alarm will be for all speeding occasions
    {

        //keeps first set of  5  temp alarms in the same place
       for (i = 0; i<messageSize*5; i++)
       {
           read_back_data[i] = *addr_pointer++;
       }
        //gets rid of the last speeding alarm and shifts them all down one to make room for the new one
       for (i = messageSize*6; i < alarmLogSize; i++)
       { // read values in flash before programming
           read_back_data[i] = *addr_pointer++;
       }

       //places the new speed alarm in the first spot of speed alarms
       for (i = messageSize*5 ; i < messageSize*6; i++)
       {
           read_back_data[i] = dataSet[i-(messageSize*5)];
       }
    }



       //Ill need this no matter what I believe

       /* Unprotecting Info Bank 0, Sector 0 */
       MAP_FlashCtl_unprotectSector(FLASH_INFO_MEMORY_SPACE_BANK0, FLASH_SECTOR0);
       /* Erase the flash sector starting CALIBRATION_START. */
       while (!MAP_FlashCtl_eraseSector(CALIBRATION_START));
       /* Program the flash with the new data. */
       while (!MAP_FlashCtl_programMemory(read_back_data,(void*) CALIBRATION_START + 4, alarmLogSize)); // leave first 4 bytes unprogrammed
       /* Setting the sector back to protected */
       MAP_FlashCtl_protectSector(FLASH_INFO_MEMORY_SPACE_BANK0, FLASH_SECTOR0);
       addr_pointer = CALIBRATION_START + 4; // point to address in flash for saved data
       for (i = 0; i < alarmLogSize; i++)
       { // read values in flash after programming
           read_back_data[i] = *addr_pointer++;
       }
}


/******************************
Name:         writeTempAlarmLog
Description:  prints last 5 temp alarms
Input:        select = rotoray encoder selections
Output:       none
Source(s):    none
*******************************/
uint8_t writeTempAlarmLog(uint8_t select, uint8_t reset)
{

    char menuItem [2][10] = {"Temp Alarm","Log"};
    uint8_t tempAlarms[100];                            //each alarm is 20 long *5 = 100 size
    uint8_t i=0, xstart=0, ystart=10, y=0;
    uint8_t s1=20;                        //represents the size of the menu option to keep them centered
    uint16_t backGround = ST7735_Color565(0, 0, 0);                     //Black background
    uint16_t textColor  = ST7735_Color565(25, 255, 255);                //text color
    uint16_t selectColor= ST7735_Color565(0, 0, 0);
    uint16_t selectBack = ST7735_Color565(25, 255, 255);

    uint8_t* addr_pointer; // pointer to address in flash for reading back values

    static uint8_t count = 0;       //used to determine what to highlight


    if(select == 3)
    {
        //3 indicates a button press and should go to next state depending on option selected
        //would need to reset count to 0
        count = 0;
        return (1);
    }


    addr_pointer = CALIBRATION_START + 4; // point to address in flash for saved data
    for (i = 0; i < 100; i++)
    { // read values in flash after programming
        tempAlarms[i] = *addr_pointer++;
    }
    i=0;


    //start printing sequence
    if(reset == 1)
        count=0;
    if(count == 0){ ST7735_FillScreen(ST7735_Color565(0, 0, 0));  count++;}                       //Background for whole screen only the first time


    //print header
    ystart = 30;
    xstart = 10;
    while(i < 10)
    {
            ST7735_DrawCharS(xstart+(11*i),ystart+(y*17), menuItem[y][i], textColor, backGround, 2);
        i++;
    }
    //prints second line of header
    i=0;
    y++;
    xstart = 63-((3*11)/2);
    while(i < 3)
    {
        ST7735_DrawCharS(xstart+(11*i),ystart+(y*17), menuItem[y][i], textColor, backGround, 2);
        i++;
    }


    //start printing alarm list
    i=0;
    ystart = 75;
    xstart = 13; //63-((s1*6)/1);
    for(y=0; y<5; y++)
    {
        i=0;
        while(i <s1)
        {
            ST7735_DrawCharS(xstart+(6*i),ystart+(y*12), tempAlarms[i+(s1*y)], textColor, backGround, 1);
            i++;
        }
    }


    return(0);              //if not button pushed return 0 to stay in menu function
}




/******************************
Name:         writeSpeedAlarmLog
Description:  prints last 5 temp alarms
Input:        select = rotoray encoder selections
Output:       none
Source(s):    none
*******************************/
uint8_t writeSpeedAlarmLog(uint8_t select, uint8_t reset)
{

    char menuItem [2][11] = {"Speed Alarm","Log"};
    uint8_t speedAlarms[100];                            //each alarm is 20 long *5 = 100 size
    uint8_t i=0, xstart=0, ystart=10, y=0;
    uint8_t s1=20;                        //represents the size of the menu option to keep them centered
    uint16_t backGround = ST7735_Color565(0, 0, 0);                     //Black background
    uint16_t textColor  = ST7735_Color565(25, 255, 255);                //text color
    uint16_t selectColor= ST7735_Color565(0, 0, 0);
    uint16_t selectBack = ST7735_Color565(25, 255, 255);

    uint8_t* addr_pointer; // pointer to address in flash for reading back values

    static uint8_t count = 0;       //used to determine what to highlight


    if(select == 3)
    {
        //3 indicates a button press and should go to next state depending on option selected
        //would need to reset count to 0
        count = 0;
        return (1);
    }


    addr_pointer = CALIBRATION_START + 4 + 100; // point to address in flash for speed saved data
    for (i = 0; i < 100; i++)
    { // read values in flash after programming
        speedAlarms[i] = *addr_pointer++;
    }
    i=0;


    //start printing sequence
    if(reset == 1)
        count=0;
    if(count == 0){ ST7735_FillScreen(ST7735_Color565(0, 0, 0));  count++;}                       //Background for whole screen only the first time


    //print header
    ystart = 30;
    xstart = 3;
    while(i < 11)
    {
            ST7735_DrawCharS(xstart+(11*i),ystart+(y*17), menuItem[y][i], textColor, backGround, 2);
        i++;
    }
    //prints second line of header
    i=0;
    y++;
    xstart = 63-((3*11)/2);
    while(i < 3)
    {
        ST7735_DrawCharS(xstart+(11*i),ystart+(y*17), menuItem[y][i], textColor, backGround, 2);
        i++;
    }


    //start printing alarm list
    i=0;
    ystart = 75;
    xstart = 13; //63-((s1*6)/1);
    for(y=0; y<5; y++)
    {
        i=0;
        while(i <s1)
        {
            ST7735_DrawCharS(xstart+(6*i),ystart+(y*12), speedAlarms[i+(s1*y)], textColor, backGround, 1);
            i++;
        }
    }


    return(0);              //if not button pushed return 0 to stay in menu function
}






//
//
//int main(void)
//{
//    void clockInit48MHzXTL(void);
//    char daysOfWeek[7][10]={"Monday","Tuesday","Wednesday","Thursday","Friday","Saturday","Sunday"};
//    char monthsOfYear[12][10]={"January","February","March","April","May","June","July","August","September","October","November","Christmas"};
//    uint8_t potato = 0;
//    /* Stop Watchdog  */
//    MAP_WDT_A_holdTimer();
//
//
//    pinInit();  //Initializes all GPIO pins
//    printf("Enter the Second\n");
//    initialize.Second = read2();
//    if (initialize.Second != 0xcc)
//    {
//        printf("Enter the Minute\n");
//        initialize.Minute = read2();   //reads keypad
//        printf("Enter the Hour\n");
//        initialize.Hour = read2();
//        printf("Enter Day of the Week\n");
//        initialize.DayOfWeek = read2();
//        printf("Enter the Date\n");
//        initialize.Date = read2();
//        printf("Enter the Month\n");
//        initialize.Month = read2();
//        printf("Enter the Year\n");
//        initialize.Year = read2();
//
//        writeI2C(); //Writes to the RTC module
//    }
//
//    printf("Enter a " "*" " to get the time and date\n");
//   while(1)
//    {
//
//       if(read1()==10)
//       {
//           printf("Enter ""1"" for time\nEnter ""2"" for date\nEnter ""3"" for Temp\n");
//           potato = read1();
//           readI2C(potato);   //Reads from the RTC module
//           if(potato == 1) sprintf(newGet,"The time is %02x:%02x:%02x", read.Hour, read.Minute, read. Second);
//           if(potato == 2) sprintf(newGet,"Today is %s %s %x, 20%x",daysOfWeek[((((read.DayOfWeek>>4)&0x0f)*10)+(read.DayOfWeek&0x0f))-1], monthsOfYear[((((read.Month>>4)&0x0f)*10)+(read.Month&0x0f))-1], read.Date, read.Year);
//           if(potato == 3) sprintf(newGet,"The Temp is %f", read.Temp);
//           printf("%s\n",newGet);
//           saveToFlash(newGet);
//           memset(newGet,0,35);
//
//           printf("Enter a ""*"" to get the time, date, or temp\n");
//       }
//       __delay_cycles(200); //Delay
//    }
//}
