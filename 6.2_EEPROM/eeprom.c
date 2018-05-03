// Library to communicate with a 25LC160 EEPROM


#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_spi.h>
#include "spi.h"
#include "eeprom.h"


#define EEPROM_PORT GPIOC
#define EEPROM_CS GPIO_Pin_10
#define EEPROM_SPI SPI2
#define EEPROM_SPEED SPI_SLOW
#define PAGESIZE 16

#define MAX_NO_WRITES(x) (PAGESIZE - (x % PAGESIZE))


enum eepromCMD
{cmdREAD = 0x03, cmdWRITE = 0x02,
	cmdWREN = 0x06, cmdWRDI = 0x04 ,
	cmdRDSR = 0x05, cmdWRSR = 0x01};


void eepromInit(void)
{
	/*Initialise CS* pin*/

	// Clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); // This should probably be changed to EEPROM_PORT somehow
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = EEPROM_CS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(EEPROM_PORT, &GPIO_InitStructure);

	/*Initialise SPI*/
	spiInit(EEPROM_SPI);
}


uint8_t eepromReadStatus()
{
	uint8_t cmd[] = {cmdRDSR , 0xff};
	uint8_t res[2];
	GPIO_WriteBit(EEPROM_PORT, EEPROM_CS ,0);
	spiReadWrite(EEPROM_SPI, res, cmd, 2, EEPROM_SPEED);
	GPIO_WriteBit(EEPROM_PORT, EEPROM_CS ,1);
	return res[1];
}


void eepromWriteEnable()
{
	uint8_t cmd = cmdWREN;
	while (WIP_READ(eepromReadStatus()));
	GPIO_WriteBit(EEPROM_PORT, EEPROM_CS, 0);
	spiReadWrite(EEPROM_SPI, 0, &cmd, 1, EEPROM_SPEED);
	GPIO_WriteBit(EEPROM_PORT, EEPROM_CS, 1);
}

void eepromWriteDisable()
{
	uint8_t cmd = cmdWRDI;
	while (WIP_READ(eepromReadStatus()));
	GPIO_WriteBit(EEPROM_PORT, EEPROM_CS, 0);
	spiReadWrite(EEPROM_SPI, 0, &cmd, 1, EEPROM_SPEED);
	GPIO_WriteBit(EEPROM_PORT, EEPROM_CS, 1);
}

void eepromWriteStatus(uint8_t status)
{
	uint8_t cmdbuf[2] = {cmdWRSR, status};
	while (WIP_READ(eepromReadStatus()));
	GPIO_WriteBit(EEPROM_PORT, EEPROM_CS, 0);
	spiReadWrite(EEPROM_SPI, 0, cmdbuf, 2, EEPROM_SPEED);
	GPIO_WriteBit(EEPROM_PORT, EEPROM_CS, 1);
}

int eepromRead(uint8_t *buf, uint8_t cnt, uint16_t offset)
{
	uint8_t cmd = cmdREAD;
	while (WIP_READ(eepromReadStatus()))
		;
	GPIO_WriteBit(EEPROM_PORT, EEPROM_CS, 0);
	spiReadWrite(EEPROM_SPI, 0, &cmd, 1, EEPROM_SPEED);
	spiReadWrite16(EEPROM_SPI, 0, &offset, 1, EEPROM_SPEED);
	spiReadWrite(EEPROM_SPI, buf, 0, cnt, EEPROM_SPEED);
	GPIO_WriteBit(EEPROM_PORT, EEPROM_CS, 1);

	return 0;
}

int eepromWrite(uint8_t *buf, uint8_t cnt, uint16_t offset)
{
	uint8_t cmd = cmdWRITE;

	// check how many bytes we can write to the page current page

	int noWrites = ((cnt > MAX_NO_WRITES(offset)) ? MAX_NO_WRITES(offset) : cnt);

	while (WIP_READ(eepromReadStatus())); // wait if eeprom is busy
	eepromWriteEnable(); // assert WEL
	while(!(WEL_READ(eepromReadStatus()))) // not necessary, but useful for debugging
		;
	GPIO_WriteBit(EEPROM_PORT, EEPROM_CS, 0);

	// Write command and offset (writing command as 16 bit does not work, so we need to do separate calls to spiReadWrite and spiReadWrite16
	spiReadWrite(EEPROM_SPI, 0, &cmd, 1, EEPROM_SPEED);
	spiReadWrite16(EEPROM_SPI, 0, &offset, 1, EEPROM_SPEED);
	// Write data
	spiReadWrite(EEPROM_SPI, 0, buf, noWrites, EEPROM_SPEED);
	GPIO_WriteBit(EEPROM_PORT, EEPROM_CS, 1);

	return noWrites;
}



