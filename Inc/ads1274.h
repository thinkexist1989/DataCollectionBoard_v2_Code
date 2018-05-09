#ifndef ADS1274_H
#define ADS1274_H

#include "spi.h"

extern int valarray[4]; //存放 ADS1274 四路模拟量24位值
extern double vout[4];  //存放 ADS1274 四路模拟量转换后的电压值
extern double v0;
extern uint8_t bitarray[12]; //SPI2数据接收缓存
extern int bAdsDataReady; //ADS1274收到数据置1

#define ADFORMAT0 PBout(5)  //format[2:0] 000 spi->dynamic
#define ADFORMAT1 PBout(6)  //            001 spi->fixed
#define ADFORMAT2 PBout(7)  

#define ADMODE0   PBout(8)  // 高分辨率模式 mode0 = 1  mode1 = 0
#define ADMODE1   PBout(9)  //                

#define ADCLKDIV  PAout(0) //时钟分频， High-resolution=>保持高电平
#define ADSYNC	  PAout(1) //同步引脚，无需同步=>保持高电平

void ADS1274_Init(void);
void ADS1274_ProcessData(void);

#endif
