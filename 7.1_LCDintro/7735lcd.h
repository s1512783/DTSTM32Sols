#ifndef LCD7735_H_
#define LCD7735_H_

#define MADCTLGRAPHICS 0x6
#define MADCTLBMP 0x2
#define ST7735_width 128
#define ST7735_height 160

// Color definitions
#define	ST7735_BLACK   0x0000
#define	ST7735_BLUE    0x001F
#define	ST7735_RED     0xF800
#define	ST7735_GREEN   0x07E0
#define ST7735_CYAN    0x07FF
#define ST7735_MAGENTA 0xF81F
#define ST7735_YELLOW  0xFFE0
#define ST7735_WHITE   0xFFFF

void ST7735_setAddrWindow (uint16_t x0 , uint16_t y0,
uint16_t x1, uint16_t y1, uint8_t madctl);
void ST7735_pushColor (uint16_t *color, int cnt);
void ST7735_init();
void ST7735_backLight(uint8_t on);
void Delay(uint32_t nTime);

#endif 
