/*
 * libnunchuk.h
 *
 *  Created on: 23 May 2018
 *      Author: m
 */

#ifndef LIBNUNCHUK_H_
#define LIBNUNCHUK_H_



// Calibration data for nunchuk (zero offsets)
#define SX_OFFSET 133
#define SY_OFFSET 123
#define AX_OFFSET 520
#define AY_OFFSET 470
#define AZ_OFFSET 697

// struct for holing nunchuk data
typedef struct
{
	// stick X and y pos
	int stickX;
	int stickY;
	// accelerometer X, Y, Z
	int accelX;
	int accelY;
	int accelZ;
	// buttons
	uint8_t buttonC;
	uint8_t buttonZ;

} nunchukTypeDef;

HAL_StatusTypeDef nunchukInit(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef nunchukRead(I2C_HandleTypeDef *hi2c, nunchukTypeDef *nunchuk);

#endif /* LIBNUNCHUK_H_ */
