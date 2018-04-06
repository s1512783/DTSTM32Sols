# include <stm32f10x_rcc.h>
# include <stm32f10x_gpio.h>
# include <stm32f10x.h>

/* Blinky + Buttons Blinking a led with a pushbutton. The button is on port A.
 * It uses polling to get the button value. The proper way to do this would be
 * with an interrupt, but I don't know how to do that yet.
 *
 * Marcin Morawski, 06.04.2018
 */


int checkDelay(uint32_t nTime);

int main(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	// Enable Peripheral Clocks
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE); // Just like for ex. 4.1, but we need to clock port A as well
	// Configure Pins
	GPIO_StructInit(&GPIO_InitStructure);
	// LEDS
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	// Pushbutton
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// Configure SysTick Timer
	if (SysTick_Config(SystemCoreClock/1000))
		while (1);


	while (1) {
		static int ledval = 0;
		// toggle led if delay time faster
		if (checkDelay(250)) {
		GPIO_WriteBit(GPIOC, GPIO_Pin_9, (ledval) ? Bit_SET : Bit_RESET);
		ledval = 1 - ledval;
		}

		if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0))
			GPIO_WriteBit(GPIOC, GPIO_Pin_8, Bit_SET);
		else
			GPIO_WriteBit(GPIOC, GPIO_Pin_8, Bit_RESET);
	}
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
