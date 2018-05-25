#include "stm32f1xx_hal.h"
#include "7735lcd.h"
#include "font.h"
#include "main.h"

// pin definitions
#define LCD_CS_Pin GPIO_PIN_0
#define LCD_CS_GPIO_Port GPIOC
#define LCD_RESET_Pin GPIO_PIN_1
#define LCD_RESET_GPIO_Port GPIOC
#define DC_Pin GPIO_PIN_2
#define DC_GPIO_Port GPIOC
#define BKL_Pin GPIO_PIN_1
#define BKL_GPIO_Port GPIOA
#define SD_CS_Pin GPIO_PIN_6
#define SD_CS_GPIO_Port GPIOC
#define SPILCD &hspi2

// LCD control
#define LCD_C RESET
#define LCD_D SET
#define ST7735_CASET 0x2A
#define ST7735_RASET 0x2B
#define ST7735_MADCTL 0x36
#define ST7735_RAMWR 0x2C
#define ST7735_RAMRD 0x2E
#define ST7735_COLMOD 0x3A

// the book or's MADVAL with 0x08, which inverts the color byte order in my display
//(I think it depends on the version of 7735 you have)

#define MADVAL(x) (((x) << 5))

// SPI handles & prototypes
SPI_HandleTypeDef hspi2;
static void LCD_MX_GPIO_Init(void);
static void LCD_MX_SPI2_Init(void);


struct ST7735_cmdBuf
{
	uint8_t command; //ST7735 command byte
	uint8_t delay; // ms delay after
	uint8_t len; // length of parameter data
	uint8_t data [16]; //parameter data
};


static struct ST7735_cmdBuf initializers[] = {
		// SWRESET Software reset
		{0x01, 150, 0, 0},
		// SLPOUT Leave sleep mode
		{0x11, 150, 0, 0},
		// FRMCTR1 , FRMCTR2 Frame Rate configuration -- Normal mode , idle
		// frame rate = fosc / (1 x 2 + 40) * (LINE + 2C + 2D)
		{0xB1 , 0, 3, {0x01 , 0x2C, 0x2D}},
		{0xB2 , 0, 3, {0x01 , 0x2C, 0x2D}},
		// FRMCTR3 Frame Rate configureation -- partial mode
		{0xB3 , 0, 6, {0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D}},
		// INVCTR Display inversion (no inversion)
		{0xB4, 0 , 1, {0x07}},
		// PWCTR1 Power control -4.6V, Auto mode
		{0xC0, 0, 3, {0xA2, 0x02, 0x84}},
		// PWCTR2 Power control VGH25 2.4C, VGSEL -10, VGH = 3 * AVDD
		{0xC1, 0, 1, {0xC5}},
		// PWCTR3 Power control, opamp current small, boost frequency
		{0xC2, 0, 2, {0x0A, 0x00}},
		// PWCTR4 Power control , BLK/2, opamp current small and medium low
		{0xC3, 0, 2, {0x8A , 0x2A}},
		// PWRCTR5, VMCTR1 Power control
		{0xC4, 0, 2, {0x8A, 0xEE}},
		{0xC5, 0, 1, {0x0E}},
		// INVOFF Don't invert display
		{0x20, 0, 0, 0},
		// Memory access directions . row address /col address , bottom to top refesh (10.1.27)
		{ST7735_MADCTL, 0, 1, {MADVAL(MADCTLGRAPHICS)}},

		/* CHECK DISPLAY MODEL (REG/GREEN/BLACK)*/

		// Color mode 16 bit (10.1.30)
		{ST7735_COLMOD, 10, 1, {0x05}},
		// Column address set 0..127
		{ST7735_CASET, 0, 4, {0x00, 0x02, 0x00, 0x7F}},
		// Row address set 0..159
		{ST7735_RASET, 0, 4, {0x00, 0x01, 0x00, 0x9F}},
		// GMCTRP1 Gamma correction
		{0xE0, 0, 16, {0x02, 0x1C, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2D,
				0x29, 0x25, 0x2B, 0x39, 0x00, 0x01, 0x03, 0x10}},
				// GMCTRP2 Gamma Polarity correction
				{0xE1, 0, 16, {0x03, 0x1d, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2D,
						0x2E, 0x2E, 0x37, 0x3F, 0x00, 0x00, 0x02, 0x10}},

						/* ADAFRUIT HAS DISPON AND NORON SWAPPED */

						// DISPON Display on
						{0x29, 100, 0, 0},
						// NORON Normal on
						{0x13, 10, 0, 0},
						// End
						{0, 0, 0, 0}
};


// low level routines
/* LcdWrite - send 8-bit data to LCD over SPI*/
static void LcdWrite(char dc, uint8_t *data, int cnt)
{
	HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, dc); // we use the DC pin to distinguish between data and control sequences
	HAL_GPIO_WritePin(DC_GPIO_Port, LCD_CS_Pin, RESET); // select LCD on SPI by pulling its pin low
	HAL_SPI_Transmit(SPILCD, data, cnt, HAL_MAX_DELAY); // send SPI data
	HAL_GPIO_WritePin(DC_GPIO_Port, LCD_CS_Pin, SET); // disassert LCD on SPI
}

/* LcdWrite - send 16-bit data to LCD over SPI*/
static void LcdWrite16(char dc, uint16_t *data, int cnt)
{
	hspi2.Init.DataSize = SPI_DATASIZE_16BIT;
	HAL_SPI_Init(&hspi2);

	HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, dc);
	/*SPI_HandleTypeDef *spiPointer = SPILCD;
	// change spi data size to 16 bit
	spiPointer->Instance->CR1 |= (~(uint16_t)SPI_DATASIZE_16BIT);*/
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, RESET);
	HAL_SPI_Transmit(SPILCD, (uint8_t *)data, cnt, HAL_MAX_DELAY); // send SPI data
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, SET);
	// change spi data size back to 8 bit
	/*spiPointer->Instance->CR1 &= (~(uint16_t)SPI_DATASIZE_16BIT); // clear DFF value
	spiPointer->Instance->CR1 |= (~(uint16_t)SPI_DATASIZE_8BIT); // set it back to 8bit, I could probably get away with just zeroing it all*/
	hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
	HAL_SPI_Init(&hspi2);
}

/* ST7735_writeCmd - send a command to the ST7735 chip*/
static void ST7735_writeCmd(uint8_t c)
{
	LcdWrite(LCD_C, &c, 1);
}


/*  ST7735_init - initialise the LCD and SPI stuff */
void ST7735_init()
{
	LCD_MX_GPIO_Init(); // initialise LCD GPIO
	LCD_MX_SPI2_Init(); // initialise LCD SPI

	const struct ST7735_cmdBuf *cmd;

	/*Initialise SPI*/
	/* set cs , reset low  - I've no idea why this needs to happen. Maybe we're resetting the configuration of the LCD chip?*/
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, SET);
	HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, SET);
	HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, SET);
	HAL_Delay(10);

	// Send initialization commands to ST7735
	for (cmd = initializers; cmd->command; cmd ++){
		LcdWrite(LCD_C, &(cmd->command), 1); // why are they dereferencing the pointer and then referencing it again?
		if (cmd->len)
			LcdWrite(LCD_D, cmd->data , cmd->len);
		if (cmd->delay)
			HAL_Delay(cmd->delay);
	}
}


static uint8_t madctlcurrent = MADVAL(MADCTLGRAPHICS); // for storing current control sequence

/* ST7735_setAddrWindow - send window in which we'll be drawing pixels */
void ST7735_setAddrWindow(uint16_t x0, uint16_t y0,
		uint16_t x1, uint16_t y1, uint8_t madctl)
{
	madctl = MADVAL(madctl);
	if (madctl != madctlcurrent){
		ST7735_writeCmd(ST7735_MADCTL);
		LcdWrite(LCD_D, &madctl, 1);
		madctlcurrent = madctl;
	}

	// set column boundaries
	ST7735_writeCmd(ST7735_CASET);
	LcdWrite16(LCD_D, &x0, 1);
	LcdWrite16(LCD_D, &x1, 1);

	// set row boundaries
	ST7735_writeCmd(ST7735_RASET);
	LcdWrite16(LCD_D, &y0, 1);
	LcdWrite16(LCD_D, &y1, 1);

	// send write command to start writing
	ST7735_writeCmd(ST7735_RAMWR);
}

/* ST7735_pushColor - write pixel color data to LCD RAM */
void ST7735_pushColor(uint16_t *color, int cnt)
{
	LcdWrite16(LCD_D, color, cnt);
}

void ST7735_fillRect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
	uint8_t x,y;
	ST7735_setAddrWindow(x0, y0, x1 - 1, y1 - 1, MADCTLGRAPHICS);
	for (x=x0; x < x1; x++) {
		for (y=y0; y < y1; y++) {
			ST7735_pushColor(&color, 1);
		}
	}
}


/* ST7735_drawChar - draws character C from ASCII bmp font defined in font.h */
void ST7735_drawChar(uint16_t x0, uint16_t y0,
		unsigned char c, uint16_t textColor, uint16_t bgColor)
{
	uint8_t i, j;

	// For some reason the Adafruit fonts seem to be rotated 90 degrees. I fixed this by swapping all the y's and x's.
	// This required defining a new memory access type. Not elegant, but works.
	ST7735_setAddrWindow(y0, x0, y0 + 9, x0 + 6, MADCTLTEXT);
	// each character is stored as 5 lines
	for (i = 0; i < 5; i++){

		uint8_t line = font[5*c + i]; // get line from font

		//draw letter and single line spacing
		for (j=0; j < 8; j++, line >>= 1){
			if (line & 0x01)
				ST7735_pushColor(&textColor, 1);
			else
				ST7735_pushColor(&bgColor, 1);
		}

		// draw line spacing
		ST7735_pushColor(&bgColor, 2);
	}

	// draw character spacing
	// ST7735_pushColor(&bgColor, 10); -- can't do this
	// For some reason it gives erratic errors in the vertical line (random pixels turned on)
	for (j = 0; j<10; j++)
		ST7735_pushColor(&bgColor, 1);
}


/* ST7735_printStringRect - print as much of str as will fit in rectangle defined by x0, y0, x1, y1.
 * Return number of chars printed
 */
uint8_t ST7735_printStringRect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
		unsigned char *str, uint16_t textColor, uint16_t bgColor)
{
	uint8_t i, j, lineLen, noLines, noWritten, endString;

	// calculate drawing space
	lineLen = (x1 - x0) / CHARWIDTH;
	noLines = (y1 - y0) / CHARHEIGHT;
	endString = 0;
	noWritten = 0;

	for (i = 0; i < noLines; i++){
		if (!endString){
			for (j = 0; j < lineLen; j++) {
				if (*str == '\n'){ // skip line if newline
					str++;
					break;
				}
				if (*str == '\0') { // stop printing chars if end of string
					endString = 1;
					break;
				}
				ST7735_drawChar(x0 + j*CHARWIDTH, y0 + i*CHARHEIGHT, *str++, textColor, bgColor);
				noWritten++;
			}
		}
		else
			break;
	}


	return noWritten;
}

void ST7735_drawVertLine(uint16_t x0, uint16_t y0,
		uint8_t length, uint8_t thickness,  uint16_t lineColor)
{
	ST7735_fillRect(x0, y0, x0 + thickness, y0 + length, lineColor);
}

void ST7735_drawHorizLine(uint16_t x0, uint16_t y0,
		uint8_t length, uint8_t thickness,  uint16_t lineColor)
{
	ST7735_fillRect(x0, y0, x0 + length, y0 + thickness, lineColor);
}

void ST7735_drawRect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
		uint8_t thickness,  uint16_t lineColor)
{
	ST7735_drawVertLine(x0, y0, y1 - y0, thickness, lineColor);
	ST7735_drawHorizLine(x0 + thickness, y1 - thickness, x1 - x0 - thickness, thickness, lineColor);
	ST7735_drawHorizLine(x0 + thickness, y0, x1 - x0 - thickness, thickness, lineColor);
	ST7735_drawVertLine(x1 - thickness, y0 + thickness, y1 - y0 - 2 * thickness, thickness, lineColor);
}

void ST7735_drawPixel(uint16_t x0, uint16_t y0, uint16_t color)
{
	ST7735_setAddrWindow(x0, y0, x0+1, y0+1, MADCTLGRAPHICS);
	ST7735_pushColor(&color, 1);
}

void ST7735_drawCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color)
{
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	ST7735_drawPixel(x0  , y0+r, color);
	ST7735_drawPixel(x0  , y0-r, color);
	ST7735_drawPixel(x0+r, y0  , color);
	ST7735_drawPixel(x0-r, y0  , color);

	while (x<y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		ST7735_drawPixel(x0 + x, y0 + y, color);
		ST7735_drawPixel(x0 - x, y0 + y, color);
		ST7735_drawPixel(x0 + x, y0 - y, color);
		ST7735_drawPixel(x0 - x, y0 - y, color);
		ST7735_drawPixel(x0 + y, y0 + x, color);
		ST7735_drawPixel(x0 - y, y0 + x, color);
		ST7735_drawPixel(x0 + y, y0 - x, color);
		ST7735_drawPixel(x0 - y, y0 - x, color);
	}
}


/* ST7735_backLight - turn LCD backlight on and off */
void ST7735_backLight(uint8_t on)
{
	if(on)
		HAL_GPIO_WritePin(BKL_GPIO_Port, BKL_Pin, SET);
	else
		HAL_GPIO_WritePin(BKL_GPIO_Port, BKL_Pin, RESET);
}

static void LCD_MX_GPIO_Init(void)
{

	GPIO_InitTypeDef GPIO_InitStruct;

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOC, LCD_CS_Pin|LCD_RESET_Pin|DC_Pin|SD_CS_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(BKL_GPIO_Port, BKL_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pins : LCD_CS_Pin LCD_RESET_Pin DC_Pin SD_CS_Pin
                           LD4_Pin LD3_Pin */
	GPIO_InitStruct.Pin = LCD_CS_Pin|LCD_RESET_Pin|DC_Pin|SD_CS_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pin : BKL_Pin */
	GPIO_InitStruct.Pin = BKL_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(BKL_GPIO_Port, &GPIO_InitStruct);

}

/* SPI2 init function */
static void LCD_MX_SPI2_Init(void)
{

	/* SPI2 parameter configuration*/
	hspi2.Instance = SPI2;
	hspi2.Init.Mode = SPI_MODE_MASTER;
	hspi2.Init.Direction = SPI_DIRECTION_2LINES;
	hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi2.Init.NSS = SPI_NSS_SOFT;
	hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
	hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi2.Init.CRCPolynomial = 10;
	if (HAL_SPI_Init(&hspi2) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}

}
