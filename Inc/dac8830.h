#ifndef DAC8830_H
#define DAC8830_H

#include "spi.h"

extern int iDacMv;

//#define CS1 PAout(4)
#define DAC_CS_Reset() HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET)//CS
#define DAC_CS_Set() HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET)//CS

void DAC8830_Init(void);
void DAC_SetValue(uint16_t val);
void DAC_SetVout(int mv);
void SPI1_WriteByte(uint16_t TxData);

#endif
