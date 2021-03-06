#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_spi.h>
#include "spi.h"

/* Loop back SPI data
 *
 * Marcin Morawski, 06.04.2018
 */

void csInit(void);

uint8_t txbuf[4], rxbuf[4];
uint16_t txbuf16[4], rxbuf16[4];

int main(void)
{

	int i, j;

	csInit(); // Initialize chip select pin PC03
	spiInit(SPI2);

	for (i = 0; i < 8; i++) {
		for (j = 0; j < 4; j++)
			txbuf [j] = i*4 + j;
		GPIO_WriteBit(GPIOC, GPIO_Pin_3, 0);
		spiReadWrite(SPI2, rxbuf, txbuf, 4, SPI_SLOW);
		GPIO_WriteBit(GPIOC, GPIO_Pin_3, 1);
		for (j = 0; j < 4; j++)
			if (rxbuf [j] != txbuf [j])
				assert_failed (__FILE__ ,__LINE__);
	}

	for (i = 0; i < 8; i++) {
		for (j = 0; j < 4; j++)
			txbuf16 [j] = i*4 + j + (i << 8);
		GPIO_WriteBit(GPIOC, GPIO_Pin_3, 0);
		spiReadWrite16(SPI2, rxbuf16, txbuf16, 4, SPI_SLOW);
		GPIO_WriteBit(GPIOC, GPIO_Pin_3, 1);
		for (j = 0; j < 4; j++)
			if (rxbuf16 [j] != txbuf16 [j])
				assert_failed (__FILE__, __LINE__);
	}

	return 0
}

/* initialise PC3 as output for SPI CS*/
void csInit(void)
{
	// Clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	// GPIO
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Spe1ed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
	/* Infinite loop */
	/* Use GDB to find out why we're here */
	while (1);
}
#endif
