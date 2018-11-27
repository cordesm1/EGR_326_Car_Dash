/*
 * ADC.h
 *
 *  Created on: Nov 27, 2018
 *      Author: nicklaskoeller
 */

#ifndef ADC_H_
#define ADC_H_

void ADCBacklightInit(void);
void updateBacklight(float normalizedADCRes, float normalizedADCResBat);



#endif /* ADC_H_ */
