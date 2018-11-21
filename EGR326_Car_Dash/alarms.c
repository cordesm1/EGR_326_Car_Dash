/*
 * alarms.c
 *
 *  Created on: Nov 21, 2018
 *      Author: corde
 */

#include "driverlib.h"



#define CALIBRATION_START 0x000200000


void saveToFlash(uint8_t dataSet[35] , uint8_t alarmType)
{
    //alarmType 1=speed   2=temp

    uint8_t i, alarmLogSize = 140 , messageSize = 35; // index
    uint8_t read_back_data[140]; // array to hold values read back from flash
    //change 35 to the length of one message

    uint8_t* addr_pointer; // pointer to address in flash for reading back values

    addr_pointer = CALIBRATION_START + 4; // point to address in flash for saving data
       for (i = messageSize; i < alarmLogSize; i++)
       { // read values in flash before programming
           read_back_data[i] = *addr_pointer++;
       }
       for (i = 0 ; i < messageSize; i++)
       {
           read_back_data[i] = dataSet[i];
       }
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
