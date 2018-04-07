# include <stm32f10x.h>
# include <stm32f10x_usart.h>
# include "usart.h"

/* Echo input through USART
 *
 * Marcin Morawski, 06.04.2018
 */


int main(void)
{
	// Start USART
	uart_open(USART1, 9600);

	// Configure SysTick Timer
	if (SysTick_Config(SystemCoreClock/1000))
		while (1);

	int c;
	while((c = uart_getc(USART1)) != -1)
		uart_putc(c, USART1);
	return 0;

}

/* We're using timeKeeper to check if the delay time of the green LED has elapsed
 * The SysTick_Handler just has to increment it
 */
static __IO uint32_t timeKeeper = 0;

int checkDelay(uint32_t nTime)
{
	if (timeKeeper >= nTime){
		timeKeeper = 0;
		return 1;
	}
	else
		return 0;
}


void SysTick_Handler(void)
{
	timeKeeper++;
}


#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
	/* Infinite loop */
	/* Use GDB to find out why we're here */
	while (1);
}
#endif
