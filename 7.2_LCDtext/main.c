#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_spi.h>
#include "spi.h"
#include "7735lcd.h"

/* Display text on a 7735 LCD
 *
 * Marcin Morawski, 21.04.2018
 */

void fillScreen(uint16_t color);

int main(void)
{
	ST7735_init();
	ST7735_backLight(1);

	fillScreen(ST7735_BLACK);

	ST7735_drawChar(10, 10, '0', ST7735_WHITE, ST7735_BLACK);
	ST7735_drawChar(ST7735_width-10, 10, '1', ST7735_WHITE, ST7735_BLACK);
	ST7735_drawChar(ST7735_width-10, ST7735_height-10, '2', ST7735_WHITE, ST7735_BLACK);
	ST7735_drawChar(10, ST7735_height-10, '3', ST7735_WHITE, ST7735_BLACK);

	unsigned char str[] = "Thank you Adafruit";

	unsigned char *ptr = str;

	while (*ptr != '\0'){
		ST7735_drawChar(ST7735_width/2, ST7735_height/2, *ptr++, ST7735_WHITE, ST7735_BLACK);
		Delay(300);
	}


	return 0;
}

void fillScreen(uint16_t color)
{
	uint8_t x,y;
	ST7735_setAddrWindow(0, 0, ST7735_width-1, ST7735_height-1, MADCTLGRAPHICS);
	for (x=0; x < ST7735_width; x++) {
		for (y=0; y < ST7735_height; y++) {
			ST7735_pushColor(&color, 1);
		}
	}
}


#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
	/* Infinite loop */
	/* Use GDB to find out why we're here */
	while (1);
}
#endif
