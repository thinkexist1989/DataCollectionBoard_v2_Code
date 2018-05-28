#include "ads1274.h"

int valarray[4];
double vout[4];
double v0;
uint8_t bitarray[12];
uint8_t sendbuf[12]={0xFF}; //任意值

int bAdsDataReady = 0;

void ADS1274_Init(void)
{
//  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, ADCLKDIV_Pin|ADFORMAT0_Pin|ADMODE0_Pin|ADMODE1_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, ADFORMAT1_Pin|ADFORMAT2_Pin, GPIO_PIN_RESET);
	
	HAL_GPIO_WritePin(GPIOD, ADSYNC_Pin, GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(GPIOD, ADSYNC_Pin, GPIO_PIN_SET);
}


/***ADS1274数据处理函数，不要在中断回调函数中处理****/
void ADS1274_ProcessData(void)
{
	uint8_t i;
	for(i=0;i<4;i++){
		if((bitarray[3*i]>>7)&0x01){
			valarray[i] = (((int)(~bitarray[3*i]) << 16)&0xFF0000) + (((int)(~bitarray[3*i+1]) << 8)&0xFF00) + (((int)(~bitarray[3*i+2]) << 0)&0xFF) + 1;
			vout[i] = -(double)valarray[i]/0x7FFFFE*2.5;
		}
		else{
			valarray[i] = (((int)bitarray[3*i] << 16)&0xFF0000) + (((int)bitarray[3*i+1] << 8)&0xFF00) + (((int)bitarray[3*i+2] << 0)&0xFF);
			vout[i] = (double)valarray[i]/0x7FFFFF*2.5;
		}
	}
	
	static uint8_t ready = 0;
	if(ready == 0){
		v0 = vout[0];
		ready=1;
	}
}

/*ADDRDY中断处理函数*/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	//UNUSED(GPIO_Pin); //为了防止某些编译器编译出错
  if(GPIO_Pin == ADDRDY_Pin){
		//HAL_SPI_TransmitReceive(&hspi2,sendbuf,bitarray,12,1);
		HAL_SPI_Receive(&hspi2,bitarray,12,1);
		bAdsDataReady = 1;		
  } 
}
