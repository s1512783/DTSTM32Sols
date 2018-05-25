#ifndef LCD7735_H_
#define LCD7735_H_

#define MADCTLGRAPHICS 0x6
#define MADCTLTEXT 0x07
#define MADCTLBMP 0x2
#define ST7735_width 128
#define ST7735_height 160
#define CHARWIDTH 6
#define CHARHEIGHT 10


// Color definitions
#define	ST7735_BLACK   0x0000
#define	ST7735_BLUE    0x001F
#define	ST7735_RED     0xF800
#define	ST7735_GREEN   0x07E0
#define ST7735_CYAN    0x07FF
#define ST7735_MAGENTA 0xF81F
#define ST7735_YELLOW  0xFFE0
#define ST7735_WHITE   0xFFFF

void ST7735_init();
void ST7735_backLight(uint8_t on);

void ST7735_setAddrWindow(uint16_t x0, uint16_t y0,
		uint16_t x1, uint16_t y1, uint8_t madctl);
void ST7735_pushColor(uint16_t *color, int cnt);
void ST7735_fillRect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
void ST7735_drawChar(uint16_t x0, uint16_t y0,
		unsigned char c, uint16_t textColor, uint16_t bgColor);
uint8_t ST7735_printStringRect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
		unsigned char *str, uint16_t textColor, uint16_t bgColor);
void ST7735_drawVertLine(uint16_t x0, uint16_t y0,
		uint8_t length, uint8_t thickness,  uint16_t lineColor);
void ST7735_drawHorizLine(uint16_t x0, uint16_t y0,
		uint8_t length, uint8_t thickness, uint16_t lineColor);
void ST7735_drawRect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
		uint8_t thickness, uint16_t lineColor);
void ST7735_drawPixel(uint16_t x, uint16_t y, uint16_t color);
void ST7735_drawCircle(uint16_t x0, uint16_t y0, uint16_t radius, uint16_t color);

#endif 
