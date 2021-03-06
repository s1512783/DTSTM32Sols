#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_spi.h>
#include "spi.h"
#include "7735lcd.h"

/* Display a single colour on a 7735 LCD
 *
 * Marcin Morawski, 15.04.2018
 */

void fillScreen(uint16_t color);

int main(void)
{
	ST7735_init();
	ST7735_backLight(1);

	while(1){
	fillScreen(ST7735_RED);
	Delay(500);
	fillScreen(ST7735_GREEN);
	Delay(500);
	fillScreen(ST7735_BLUE);
	Delay(500);
	}

	return 0;
}

void fillScreen(uint16_t color)
{
	uint8_t x,y;
	ST7735_setAddrWindow (0, 0, ST7735_width-1, ST7735_height-1, MADCTLGRAPHICS);
	for (x=0; x < ST7735_width ; x++) {
		for (y=0; y < ST7735_height ; y++) {
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
