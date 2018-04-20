#ifndef 7735LCD_H_
#define 7735LCD_H_

#define MADCTLGRAPHICS 0x6
#define MADCTLBMP 0x2
#define ST7735_width 128
#define ST7735_height 160

void ST7735_setAddrWindow (uint16_t x0 , uint16_t y0,
uint16_t x1, uint16_t y1, uint8_t madctl);
void ST7735_pushColor ( uint16_t *color, int cnt);
void ST7735_init();
void ST7735_backLight(uint8_t on);

#endif 
