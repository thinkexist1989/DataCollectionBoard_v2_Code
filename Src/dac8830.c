#include "dac8830.h"

int iDacMv = 0;

void DAC8830_Init(void)
{
	//SPI1_Init();
}

void DAC_SetValue(uint16_t val)
{
	DAC_CS_Reset();
	SPI1_WriteByte(val);
	DAC_CS_Set();
}


void DAC_SetVout(int mv)
{
	uint16_t val = (double)mv/2500.0*65536;
	DAC_SetValue(val);
}

void SPI1_WriteByte(uint16_t TxData)
{	
	uint8_t TxBuf[2];
	TxBuf[0] = (TxData >> 8)&0xFF;
	TxBuf[1] = TxData &0xFF;
	HAL_SPI_Transmit(&hspi1, TxBuf, 2 ,1000);
}
