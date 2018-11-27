/*
 * alarms.h
 *
 *  Created on: Nov 21, 2018
 *      Author: corde
 */

#ifndef ALARMS_H_
#define ALARMS_H_

void saveToFlash(uint8_t dataSet[35] , uint8_t alarmType);
uint8_t writeTempAlarmLog(uint8_t select);
uint8_t writeSpeedAlarmLog(uint8_t select);


#endif /* ALARMS_H_ */
