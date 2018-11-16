/*
 * speed.h
 *
 *  Created on: Nov 16, 2018
 *      Author: corde
 */

#ifndef SPEED_H_
#define SPEED_H_

void initMotorPWM(void);
void initTimer32For100us(void);
void initHallEffectPins(void);
void changeMotorPWMspeed(uint8_t motorSpeed);



#endif /* SPEED_H_ */
