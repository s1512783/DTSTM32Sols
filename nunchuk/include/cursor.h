/*
 * cursor.h
 *
 *  Created on: 25 May 2018
 *      Author: m
 */

#ifndef CURSOR_H_
#define CURSOR_H_

#include "stm32f1xx_hal.h"
#include "font.h"

// Physical parameters for cursor
#define TIME_SCALER 1
#define STICK_SCALER 20
#define AX_SCALER 18
#define	AY_SCALER 18
#define FRICTION_ACCEL 50
#define DAMPING_ACCEL 10
#define DAMPING_SCALER 1
#define CURSOR_POS_SCALER 1500

/*
#define TIME_SCALER 1
#define STICK_SCALER 10
#define AX_SCALER 8
#define	AY_SCALER 8
#define FRICTION_ACCEL 25
#define DAMPING_ACCEL 5
#define DAMPING_SCALER 1
#define CURSOR_POS_SCALER 1000
*/

// struct for storing all cursor parameters
typedef struct {
	int posX;
	int posY;
	int velocityX;
	int velocityY;
	int accelerationX;
	int accelerationY;
	struct {
		unsigned char character;
		uint16_t textColor;
		uint16_t bgColor;
	} representation;
} cursor_t;

cursor_t cursorInit(int x0, int y0, unsigned char character, uint16_t textColor, uint16_t bgColor);
void cursorSetPos(cursor_t *c, int x, int y);
void cursorPush(cursor_t *c, int accelX, int accelY);
void cursorUpdate(cursor_t *c);



#endif /* CURSOR_H_ */
