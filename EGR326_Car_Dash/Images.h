/*
 * Images.h
 *
 *  Created on: Oct 5, 2018
 *      Author: cordesM to hold images
 */


#ifndef IMAGES_H_
#define IMAGES_H_

//Menu prints
uint8_t idleScreen(uint8_t select, float temp, uint8_t speed,uint8_t *timeArray);
uint8_t writeMenu(uint8_t select);
uint8_t writeErrorMenu(uint8_t select);
uint8_t setTimeSubMenu(uint8_t select, uint8_t *newTime, uint8_t *oldTime);
uint8_t setDateSubMenu(uint8_t select , uint8_t *newDate);
void topBannerPrint(float temp, uint8_t speed, uint8_t *timeArray);
void systick_delay_ms(uint32_t delay_time); //Function to wait "delay_time" ms
void systick_delay_us(uint32_t delay_time);//wait time in us. There is a max input
uint8_t hourCounter(uint8_t count, uint8_t sel, uint8_t digit);


//pin and general Initalizations
void rotaryPinInit(void);   //enables pins for rotary ecoder
void Systick_Init(void);    //Function to initialize the sysTick Timer


//print images to screen
void buddyCorp();

void countCorners();
void writeEGR();
void heihei();






#endif /* IMAGES_H_ */
