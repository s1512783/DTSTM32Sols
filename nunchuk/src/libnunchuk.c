/*
 * libnunchuk.c
 *
 *  Created on: 23 May 2018
 *      Author: m
 */
#include "stm32f1xx_hal.h"
#include "stdint.h"
#include "libnunchuk.h"

#define NUNCHUK_ADDRESS 0xA4

HAL_StatusTypeDef returnValue;

/*
 * nunchukInit - initializes a Wii Nunchuk adapter connected to hi2c
 */
HAL_StatusTypeDef nunchukInit(I2C_HandleTypeDef *hi2c)
{
	uint8_t initbuf1[] = {0xF0, 0x55};
	uint8_t initbuf2[] = {0xFB, 0x00};

	/*	while ((returnValue = HAL_I2C_Master_Transmit(hi2c, NUNCHUK_ADDRESS,  initbuf1, 2, HAL_MAX_DELAY)) != HAL_OK)
		;*/

	returnValue = HAL_I2C_Master_Transmit(hi2c, NUNCHUK_ADDRESS,  initbuf1, 2, HAL_MAX_DELAY);
	if(returnValue != HAL_OK)
		return returnValue;

	HAL_Delay(1);
	/*
	while ((returnValue = HAL_I2C_Master_Transmit(hi2c, NUNCHUK_ADDRESS,  initbuf2, 2, HAL_MAX_DELAY)) != HAL_OK)
			;*/

	returnValue = HAL_I2C_Master_Transmit(hi2c, NUNCHUK_ADDRESS,  initbuf2, 2, HAL_MAX_DELAY);
	if(returnValue != HAL_OK)
		return returnValue;

	return HAL_OK;
}

/*
 * nunchukRead - reads data from a Wii Nunchuk connected to hi2c and saves in nunchuk struct
 */
HAL_StatusTypeDef nunchukRead(I2C_HandleTypeDef *hi2c, nunchukTypeDef *nunchuk)
{
	uint8_t tempbuf[6];

	/*	while ((returnValue = HAL_I2C_Master_Transmit(hi2c, NUNCHUK_ADDRESS, 0x00, 1, HAL_MAX_DELAY)) != HAL_OK)
		;*/

	returnValue = HAL_I2C_Master_Transmit(hi2c, NUNCHUK_ADDRESS, 0x00, 1, HAL_MAX_DELAY);

	if(returnValue != HAL_OK)
		return returnValue;

	HAL_Delay(1);


	/*	while ((returnValue = HAL_I2C_Master_Receive(hi2c, NUNCHUK_ADDRESS, tempbuf, 6, 10)) != HAL_OK)
		;*/
	returnValue = HAL_I2C_Master_Receive(hi2c, NUNCHUK_ADDRESS, tempbuf, 6, HAL_MAX_DELAY);
	if(returnValue != HAL_OK)
		return returnValue;

	nunchuk->stickX = tempbuf[0] - SX_OFFSET;
	nunchuk->stickY = tempbuf[1] - SY_OFFSET;
	nunchuk->accelX = ((tempbuf[2]<<2) | (tempbuf[5] & 0x0C) >> 2) - AX_OFFSET;
	nunchuk->accelY = ((tempbuf[3]<<2) | (tempbuf[5] & 0x30) >> 4) - AY_OFFSET;
	nunchuk->accelZ = ((tempbuf[4]<<2) | (tempbuf[5] & 0xC0) >> 6) - AZ_OFFSET;
	nunchuk->buttonC = ((tempbuf[5] & 0x02) ? 0 : 1);
	nunchuk->buttonZ = ((tempbuf[5] & 0x01) ? 0 : 1);

	return HAL_OK;
}
