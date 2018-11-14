/*
 * RTC.h
 *
 *  Created on: Nov 13, 2018
 *      Author: nicklaskoeller
 */

#ifndef RTC_H_
#define RTC_H_



struct bcd
{
         uint8_t Second;
         uint8_t Minute;
         uint8_t Hour;
         uint8_t DayOfWeek;
         uint8_t Date;
         uint8_t Month;
         uint8_t Year;
         float Temp;
};

extern struct bcd initialize;
extern struct bcd read;

void writeI2C(void);    //writes date to the RTC
void readI2C(uint8_t mode);     //reads to from the RTC

#endif /* RTC_H_ */
