/*
 * ultraSonic.h
 *
 *  Created on: Nov 20, 2018
 *      Author: corde
 */

#ifndef ULTRASONIC_H_
#define ULTRASONIC_H_

void timerA2_init(void);                     //initalizes timerA for ultrasonic Sensor
void ultraSonicPinInit(void);                //init GPIO pins and ect
void triggerUltraSonic(void);                //triggers 10us pulse to trig p5.1


#endif /* ULTRASONIC_H_ */
