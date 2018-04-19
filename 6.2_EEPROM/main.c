#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_spi.h>
#include "spi.h"
#include "eeprom.h"

/* Connect to MCP 25LC160 EEPROM and verify functionality
 *
 * Marcin Morawski, 15.04.2018
 */


#define BUFFSIZE 25


char writeBuf[BUFFSIZE] = "I <3 SPI, SPI is the best";
uint8_t  readBuf[BUFFSIZE], eepromStatus;


int main(void)
{
	int i, j;

	eepromInit();

	/* first check eeprom status register access */

	eepromWriteEnable();

	if (WEL_READ(eepromStatus = eepromReadStatus())) // check if WREN operation worked
		eepromWriteDisable();
	else
		assert_failed(__FILE__, __LINE__);

	if (!WEL_READ((eepromStatus = eepromReadStatus()))) // check if WRDI operation worked
		eepromWriteStatus(WEL | BP0 | BP1);
	else
		assert_failed(__FILE__, __LINE__);

	if (!((eepromStatus = eepromReadStatus()) & (WEL | BP0 | BP1))) // check if WRSR operation worked
		eepromWriteStatus(0x00); // reset status word for subsequent operations
	else
		assert_failed(__FILE__, __LINE__);


	/* then try writing and reading */


	// Byte write
	eepromWrite(writeBuf, 1, 13);
	eepromRead(readBuf, 1, 13);

	if (readBuf[0] != writeBuf[0])
		assert_failed(__FILE__, __LINE__);

	// Page write

	int noWrites = eepromWrite(writeBuf, BUFFSIZE, 10);
	eepromRead(readBuf, BUFFSIZE, 10);


	for (i=0; i<noWrites; i++) // Check if write and read worked
		if (readBuf[i] != writeBuf[i])
			assert_failed(__FILE__, __LINE__);


	while (1)
		;

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
