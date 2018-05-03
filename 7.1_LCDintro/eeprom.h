#ifndef EEPROM_H_
#define EEPROM_H_
// Status register bits
#define WIP 0x01
#define WEL 0x02
#define BP0 0x04
#define BP1 0x08

// SR checking macros
#define WIP_READ(x) ((x) & WIP)
#define WEL_READ(x) ((x) & WEL)

void eepromInit();
void eepromWriteEnable();
void eepromWriteDisable();
uint8_t eepromReadStatus();
void eepromWriteStatus(uint8_t status);
int eepromWrite(uint8_t *buf, uint8_t cnt, uint16_t offset);
int eepromRead(uint8_t *buf, uint8_t cnt, uint16_t offset);
#endif
