/*
 * libnunchuk.h
 *
 *  Created on: 23 May 2018
 *      Author: m
 */

#ifndef LIBNUNCHUK_H_
#define LIBNUNCHUK_H_

// struct for holing nunchuk data
typedef struct
{
	// stick X and y pos
	uint8_t stickX;
	uint8_t stickY;
	// accelerometer X, Y, Z
	uint16_t accelX;
	uint16_t accelY;
	uint16_t accelZ;
	// buttons
	uint8_t buttonC;
	uint8_t buttonZ;

} nunchukTypeDef;

HAL_StatusTypeDef nunchukInit(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef nunchukRead(I2C_HandleTypeDef *hi2c, nunchukTypeDef *nunchuk);

#endif /* LIBNUNCHUK_H_ */
