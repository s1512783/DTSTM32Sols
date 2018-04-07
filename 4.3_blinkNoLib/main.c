# include <stm32f10x.h>

/* Button blinking LEDs without using the standard peripheral library
 * Marcin Morawski, 06.04.2018
 */



main(void)
{
	// enable clock to GPIOA, GPIOC
	RCC->APB2ENR |= 0x00000014;
	// configure button
	GPIOA->CRL = ((GPIOA->CRL & (~0x0F)) | (0x04));
	// configure led
	GPIOC->CRH = ((GPIOC->CRH & (~0xFF)) | (0x22));


	while (1)
	{
		if (GPIOA->IDR & (0x00000001)) // read pushbutton state
			GPIOC->BSRR |= 0x00000200;
		else
			GPIOC->BRR |= 0x00000200;
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
