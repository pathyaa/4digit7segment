/*
 * segment.h
 *
 *  Created on: Feb 27, 2024
 *      Author: onste
 */

#ifndef INCLUDE_SEGMENT_H_
#define INCLUDE_SEGMENT_H_

#include "main.h"

extern const uint8_t digit[10];
extern bool isDispShift;
extern uint8_t disp_step;

void shiftSegment(uint8_t data);
void dispData(uint8_t data, uint8_t pos);
bool dispIntNumber(uint16_t data);
bool dispCharacter(uint8_t data, uint8_t pos);
void dispError(void);
bool dispStatic(uint16_t data);
bool dispDynamic(uint16_t data);
void dispCountUntil9999Test(void);

#endif /* INCLUDE_SEGMENT_H_ */
