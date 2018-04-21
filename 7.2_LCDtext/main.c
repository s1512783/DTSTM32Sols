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

int main(void)
{
	ST7735_init();
	ST7735_backLight(1);

	// Test screen borders

	ST7735_fillRect(0, 0, ST7735_width, ST7735_height, ST7735_BLUE); // blue backgruond to see the text bgnd

	ST7735_drawChar(0, 0, 'A', ST7735_WHITE, ST7735_BLACK);
	ST7735_drawChar(ST7735_width-CHARWIDTH, 0, 'B', ST7735_WHITE, ST7735_BLACK);
	ST7735_drawChar(ST7735_width-CHARWIDTH, ST7735_height-CHARHEIGHT, 'C', ST7735_WHITE, ST7735_BLACK);
	ST7735_drawChar(0, ST7735_height-CHARHEIGHT, 'D', ST7735_WHITE, ST7735_BLACK);

	// Test single char print
	unsigned char str1[] = "Thank you Adafruit";

	unsigned char *ptr = str1;

	while (*ptr != '\0'){
		ST7735_drawChar(ST7735_width/2, ST7735_height/2, *ptr++, ST7735_WHITE, ST7735_BLACK);
		Delay(300);
	}

	// Test line print

	ST7735_fillRect(0, 0, ST7735_width, ST7735_height, ST7735_BLUE);

	unsigned char str2[] = "The quick brown fox\n"
			"jumps over the lazy\ndog";

	ST7735_printStringRect(0, 0, ST7735_width - 1, ST7735_height - 1, str2, ST7735_WHITE, ST7735_BLACK);

	Delay(500);

	// Test shape prints


	ST7735_fillRect(0, 0, ST7735_width, ST7735_height, ST7735_BLACK);

	ST7735_drawVertLine(ST7735_width/2, 0, ST7735_height, 2, ST7735_WHITE);

	ST7735_drawHorizLine(0, ST7735_height/2, ST7735_width, 2, ST7735_WHITE);

	Delay(500);

	ST7735_fillRect(0, 0, ST7735_width, ST7735_height, ST7735_BLACK);

	ST7735_drawRect(10, ST7735_height - 50, ST7735_width - 10, ST7735_height-10, 2, ST7735_WHITE);

	unsigned char str3[] = "The quick brown fox "
			"jumps over\nthe lazy dog";

	ST7735_printStringRect(14, ST7735_height - 46, ST7735_width - 14, ST7735_height-14, str3, ST7735_WHITE, ST7735_BLACK);

	uint16_t radius = 30;

	while(1){
		if(!radius){
			radius = 30;
			ST7735_fillRect(ST7735_width/2 - 30, 25, ST7735_width/2 + 30, 85, ST7735_BLACK);
		}

		ST7735_drawCircle(ST7735_width/2, 55, radius = radius - 5, ST7735_WHITE);
		Delay(100);
	}

	return 0;
}


#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
	/* Infinite loop */
	/* Use GDB to find out why we're here */
	while (1);
}
#endif
