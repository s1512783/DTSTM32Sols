/*
 * cursor.c
 *
 *  Created on: 25 May 2018
 *      Author: m
 */

#include "cursor.h"
#include "7735lcd.h"
#include "stm32f1xx_hal.h"




/* cursorInit - initialise a new cursor
 * x0 - initial x position
 * y0 - initial y position
 * character - char which will be used to display the cursor
 * textColor - colour of cursor text
 * bgColor - colour of cursor background
 */
cursor_t cursorInit(int x0, int y0, unsigned char character, uint16_t textColor, uint16_t bgColor)
{
	cursor_t c = {0};
	c.posX = x0;
	c.posY = y0;
	c.representation.character = character;
	c.representation.textColor = textColor;
	c.representation.bgColor = bgColor;

	return c;
}




/* cursorSetPos - sets the position of a cursor
 *  cursor - cursor struct
 *	x - position x-coordinate
 *	y - position y-coordinate
 */

void cursorSetPos(cursor_t *c, int x, int y)
{
	c->posX = x;
	c->posY = y;
	c->velocityX = 0;
	c->velocityY = 0;
	c->accelerationX = 0;
	c->accelerationY = 0;
	cursorUpdate(c);
}

/* cursorUpdate - updates the position, velocity and acceleration of the cursor
 * c - cursor struct
 */
void cursorUpdate(cursor_t *c)
{
	// calculate accelerations on the cursor. Friction only acts if object is not stationary and is always opposing motion
	if (c->velocityX > 0)
		c->accelerationX = c->accelerationX - FRICTION_ACCEL - DAMPING_ACCEL * c->velocityX / DAMPING_SCALER;
	else if (c->velocityX < 0)
		c->accelerationX = c->accelerationX + FRICTION_ACCEL - DAMPING_ACCEL * c->velocityX  / DAMPING_SCALER;
	if(c->velocityY > 0)
		c->accelerationY = c->accelerationY - FRICTION_ACCEL -  DAMPING_ACCEL * c->velocityY  / DAMPING_SCALER;
	if(c->velocityY < 0)
		c->accelerationY = c->accelerationY + FRICTION_ACCEL - DAMPING_ACCEL * c->velocityY / DAMPING_SCALER;

	// save initial velocities
	int velocityXInit = c->velocityX;
	int velocityYInit = c->velocityY;

	// calculate new velocities
	c->velocityX = c->velocityX + c->accelerationX * TIME_SCALER;
	c->velocityY = c->velocityY + c->accelerationY * TIME_SCALER;

	// check if the velocity changed sign after update - if it did, then the cursor should have
	// stopped along the way and the friction force should have stopped acting
	if (((velocityXInit * (c->velocityX)) < 0)){
		c->accelerationX = 0;
		c->velocityX = 0;
	}
	if (((velocityYInit * (c->velocityY)) < 0)){
		c->accelerationY = 0;
		c->velocityY = 0;
	}

	// finally, update the displayed position of the cursor
	//ST7735_drawChar(c->posX, c->posY, font[0], c->representation.textColor, c->representation.bgColor);

	c->posX = c->posX + (c->velocityX * TIME_SCALER / CURSOR_POS_SCALER);
	c->posY = c->posY + (c->velocityY * TIME_SCALER / CURSOR_POS_SCALER);

	// Limit position to size of display
	if (c->posX < 0) {
		c->posX = 0;
		c->velocityX = 0;
		c->accelerationX = 0;
	}
	if (c->posX > ST7735_width - CHARWIDTH){
		c->posX = ST7735_width - CHARWIDTH;
		c->velocityX = 0;
		c->accelerationX = 0;
	}

	if (c->posY < 0) {
		c->posY = 0;
		c->velocityY = 0;
		c->accelerationY = 0;
	}
	if (c->posY > ST7735_height - CHARHEIGHT){
		c->posY = ST7735_height - CHARHEIGHT;
		c->velocityY= 0;
		c->accelerationY = 0;
	}

	//ST7735_drawChar(c->posX, c->posY, c->representation.character, c->representation.textColor, c->representation.bgColor);
}

/* cursorPush - gives a "push" to the cursor i.e. updates its acceleration
 * c - cursor struck
 * accelX - acceleration in the X direction
 * accelY - acceleration in the Y direction
 */
void cursorPush(cursor_t *c, int accelX, int accelY)
{
	c->accelerationX = accelX;
	c->accelerationY = accelY;
}
