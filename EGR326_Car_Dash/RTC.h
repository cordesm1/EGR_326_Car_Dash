/*
 * RTC.h
 *
 *  Created on: Nov 13, 2018
 *      Author: nicklaskoeller
 *      Notes: All functions are designed for RTC on P1.6 and P1.7
 */

#ifndef RTC_H_
#define RTC_H_



//struct bcd
//{
//         uint8_t Second;
//         uint8_t Minute;
//         uint8_t Hour;
//         uint8_t DayOfWeek;
//         uint8_t Date;
//         uint8_t Month;
//         uint8_t Year;
//         float Temp;
//};
//
//extern struct bcd initialize;
//extern struct bcd read;

void initRTC(void);                     //Must call to init RTC on P3.6 and P3.7
void writeI2C(void);                    //writes date to the RTC
//void readI2C(uint8_t mode);           //reads to from the RTC
void writeFullRTC(uint8_t *timeArray);  //writes all data to RTC
void writeTimeOnly(uint8_t *timeArray); //writes only minutes and hours to RTC
void writeDateOnly(uint8_t *timeArray); //writes only xx/xx/xx to RTC
void readFullRTC(uint8_t *timeArray);   //reads all time and date from rtc
void readTimeIn(uint8_t *timeArray);    //Reads only time in minutes and hours in
float readRTCtemp(float RTCtemp);       //Reads only temperature

#endif /* RTC_H_ */
