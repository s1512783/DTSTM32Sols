
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 ** This notice applies to any and all portions of this file
 * that are not between comment pairs USER CODE BEGIN and
 * USER CODE END. Other portions of this file, whether
 * inserted by the user or by software development tools
 * are owned by their respective copyright owners.
 *
 * COPYRIGHT(c) 2018 STMicroelectronics
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal.h"

/* USER CODE BEGIN Includes */
#include "libnunchuk.h"
#include "cursor.h"
#include "7735lcd.h"
#include <stdlib.h>
#include <string.h>
/* USER CODE END Includes */
#define BUTTON_DEBOUNCE_TIME 500
#define abs(x) (x>0 ? x : -x)

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi2;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
nunchukTypeDef nunchuk;
HAL_StatusTypeDef nStatus;
uint8_t nunchukRXbuf[6];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI2_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_I2C1_Init(void);


unsigned char *NunchukValString(void *, unsigned char *str);

/**
 * @brief  The application entry point.
 *
 * @retval None
 */
int main(void)
{


	/* MCU Configuration----------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */

	HAL_Init();

	/* Configure the system clock */
	SystemClock_Config();


	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_SPI2_Init();
	MX_USART1_UART_Init();
	MX_I2C1_Init();

	// Initialise devices
	ST7735_init();
	nStatus = nunchukInit(&hi2c1);
	if (nStatus != HAL_OK)
		Error_Handler();

	uint32_t buttonDebounceTick = 0;




	// Display initial message

/*	unsigned char msg[] = "SX: \nSY: \nAX: \nAY: \nAZ: \nBC: \nBZ\n";
	unsigned char msg2[6];*/

	ST7735_backLight(1);
	ST7735_fillRect(0, 0, ST7735_width, ST7735_height, ST7735_BLACK);
	//ST7735_printStringRect(0, 0, 35, 70, msg, ST7735_WHITE, ST7735_BLACK);

	// Initialise cursor
	cursor_t cursorX = cursorInit(ST7735_width/2, ST7735_height/2, 'x', ST7735_WHITE, ST7735_BLACK);

	// Initialise drawing stuff

	// Array of possible colors
	uint16_t colors[] = {
			ST7735_WHITE,
			ST7735_BLACK,
			ST7735_RED,
			ST7735_BLUE,
			ST7735_CYAN,
			ST7735_GREEN,
			ST7735_MAGENTA,
			ST7735_YELLOW
	};

	// Pointer to currenly used color
	uint16_t *colorLineCurrent = colors;

	// Size of color line
	int colorLineSize = 5;

	// Display current color in left corner
	ST7735_fillRect(0,0, 10, 10, *colorLineCurrent);


	while (1)
	{
		// Read nunchuk data
		nStatus = nunchukRead(&hi2c1, &nunchuk);
		if (nStatus != HAL_OK)
			Error_Handler();

		colorLineSize = abs(nunchuk.accelZ) / 7;

/*		if(colorLineSize == 0)
			colorLineSize = 1;*/

		// On press of button C...
		if (nunchuk.buttonC){
			// Check for bounce/button pressed too soon
			if((HAL_GetTick() - buttonDebounceTick) < BUTTON_DEBOUNCE_TIME)
				;
			else {
				// Increment color pointer
				if (*colorLineCurrent != ST7735_YELLOW)
					colorLineCurrent++;
				else
					colorLineCurrent = colors;

				// Change displayed current color
				ST7735_fillRect(0,0, 10, 10, *colorLineCurrent);
				buttonDebounceTick = HAL_GetTick();
			}
		}

		// When z is not pressed...
		if (nunchuk.buttonZ){
			// Clear the previous cursor position
			/*			ST7735_fillRect(cursorX.posX, cursorX.posY,
					cursorX.posX + colorLineSize, cursorX.posY  + colorLineSize, ST7735_BLACK);*/
			// Draw in the position of the cursor
			ST7735_fillRect(cursorX.posX, cursorX.posY,
					cursorX.posX + colorLineSize, cursorX.posY  + colorLineSize, *colorLineCurrent);
		}

		// Change cursor position
		cursorPush(&cursorX, nunchuk.stickX * STICK_SCALER + nunchuk.accelX * AX_SCALER, -(nunchuk.stickY * STICK_SCALER + nunchuk.accelY * AY_SCALER));
//		cursorPush(&cursorX, nunchuk.stickX * STICK_SCALER, -nunchuk.stickY * STICK_SCALER);
		cursorUpdate(&cursorX);

		/*// Draw in the position of the cursor
		ST7735_fillRect(cursorX.posX, cursorX.posY,
				cursorX.posX + colorLineSize, cursorX.posY  + colorLineSize, *colorLineCurrent);*/



		//ST7735_printStringRect(28, 0, 70, 10, NunchukValString((&nunchuk)->stickX, msg2), ST7735_WHITE, ST7735_BLACK);
		//ST7735_printStringRect(28, 10, 70, 20, NunchukValString((&nunchuk)->stickY, msg2), ST7735_WHITE, ST7735_BLACK);
		//ST7735_printStringRect(28, 20, 70, 30, NunchukValString((&nunchuk)->accelX, msg2), ST7735_WHITE, ST7735_BLACK);
		//ST7735_printStringRect(28, 30, 70, 40, NunchukValString((&nunchuk)->accelY, msg2), ST7735_WHITE, ST7735_BLACK);

	//	ST7735_printStringRect(28, 40, 70, 50, NunchukValString((&nunchuk)->accelZ, msg2), ST7735_WHITE, ST7735_BLACK);
		//ST7735_printStringRect(28, 50, 70, 60, (nunchuk.buttonC ? "YES\n" : "NO\n"), ST7735_WHITE, ST7735_BLACK);
		//ST7735_printStringRect(28, 60, 70, 70, (nunchuk.buttonZ ? "YES\n" : "NO\n"), ST7735_WHITE, ST7735_BLACK);


		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */

	}
	/* USER CODE END 3 */

}
/* NunchukValString(void *nunchukval) - converts a nunchuk value into somthing that stringprintrect can print easily
 * nunchukval - any of the nunchuk values
 */
unsigned char *NunchukValString(void *nunchukval, unsigned char *str)
{

	itoa((int)nunchukval, str, 10);
	int len = strlen(str);
	str[len] = '\n';
	str[len + 1] = '\0';

	return str;
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{

	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;

	/**Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = 16;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}

	/**Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}

	/**Configure the Systick interrupt time
	 */
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

	/**Configure the Systick
	 */
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* I2C1 init function */
static void MX_I2C1_Init(void)
{

	hi2c1.Instance = I2C1;
	hi2c1.Init.ClockSpeed = 100000;
	hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c1.Init.OwnAddress1 = 0;
	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c1.Init.OwnAddress2 = 0;
	hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(&hi2c1) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}

}

/* SPI2 init function */
static void MX_SPI2_Init(void)
{

	/* SPI2 parameter configuration*/
	hspi2.Instance = SPI2;
	hspi2.Init.Mode = SPI_MODE_MASTER;
	hspi2.Init.Direction = SPI_DIRECTION_2LINES;
	hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi2.Init.NSS = SPI_NSS_SOFT;
	hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
	hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi2.Init.CRCPolynomial = 10;
	if (HAL_SPI_Init(&hspi2) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}

}

/* USART1 init function */
static void MX_USART1_UART_Init(void)
{

	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart1) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}

}

/** Configure pins as 
 * Analog
 * Input
 * Output
 * EVENT_OUT
 * EXTI
 */
static void MX_GPIO_Init(void)
{

	GPIO_InitTypeDef GPIO_InitStruct;

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOC, LCD_CS_Pin|LCD_RESET_Pin|DC_Pin|SD_CS_Pin
			|LD4_Pin|LD3_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(BKL_GPIO_Port, BKL_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pins : LCD_CS_Pin LCD_RESET_Pin DC_Pin SD_CS_Pin
                           LD4_Pin LD3_Pin */
	GPIO_InitStruct.Pin = LCD_CS_Pin|LCD_RESET_Pin|DC_Pin|SD_CS_Pin
			|LD4_Pin|LD3_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pin : BKL_Pin */
	GPIO_InitStruct.Pin = BKL_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(BKL_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @param  file: The file name as string.
 * @param  line: The line in file as a number.
 * @retval None
 */
void _Error_Handler(char *file, int line)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	while(1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{ 
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
