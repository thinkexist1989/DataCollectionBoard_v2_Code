/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_hal.h"
#include "crc.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */

/***********有几点注意事项***********
*1. ADS1274 SPI通信必须配置为全双工master，要不然数据就不对
*2. DMA通讯的sendbuf一定要全局变量或静态变量，因为是放到内存里慢慢发，临时变量数就不对了
*************************************/
#include "oled.h"
#include "ads1274.h"
#include "dac8830.h"

#include <string.h>
#include <math.h>

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
void InitWIFI(void)
{
			uint8_t tempbuf[256]={'\0'};
			HAL_Delay(5000);
			sprintf((char*)tempbuf,"AT+CIPSTART=\"TCP\",\"192.168.1.100\",8080\r\n");		 //连接热TCPIP服务器
			HAL_UART_Transmit(&huart2,tempbuf,100,0xFFFF);
			
			HAL_Delay(1000);
			memcpy(tempbuf,'\0',256);
			sprintf((char*)tempbuf,"AT+CIPMODE=1\r\n");		 //1透传模式
			HAL_UART_Transmit(&huart2,tempbuf,100,0xFFFF);
			
			HAL_Delay(1000);
			memcpy(tempbuf,'\0',256);
			sprintf((char*)tempbuf,"AT+CIPSEND\r\n");		 //连接热TCPIP服务器
			HAL_UART_Transmit(&huart2,tempbuf,100,0xFFFF);
			HAL_Delay(1000);
			
}
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
uint8_t bDisplay = 0;

/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */
	uint8_t disp[256]={0};
  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_CRC_Init();
  MX_SPI1_Init();
  MX_SPI2_Init();
  MX_SPI3_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_TIM3_Init();

  /* USER CODE BEGIN 2 */
	OLED_ShowString(0,3,(uint8_t*)"Initializing");   //装逼用
	OLED_Init();
	OLED_Clear();
	//ADS1274_Init();
	HAL_UART_Receive_IT(&huart1, (uint8_t *)(&RxBuf[uart.iRxPtr]), 1);
	HAL_TIM_Base_Start_IT(&htim3);
	
	
	//InitWIFI();
	OLED_Clear();
	OLED_ShowString(0,3,(uint8_t*)"Complete!!!"); //装逼用
	HAL_Delay(1000);
	
	iDacMv = 10;
	DAC_SetVout(iDacMv);
	
	
	uart.bSended2 = TRUE;
	uart.bSended1 = TRUE;

	
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
		if(bDisplay){
			bDisplay = 0;
			
			OLED_ShowString(0,0,(uint8_t*)"GFET Nanosensor");
			sprintf((char* )disp,"Vg: %-7.3lf mV",vout[1]*1000);
			//sprintf((char* )disp,"VAL: %d",123);
		
			OLED_ShowString(0,3,disp); 
		  
			double val = 0.17712*pow((0.999884*v0-vout[0])/(vout[0]-1.064789*v0),0.69558);
			sprintf((char* )disp,"CI: %-7.5lf nM",val);
			OLED_ShowString(0,6,disp);
		}

		if(bAdsDataReady){
			bAdsDataReady = 0;
			ADS1274_ProcessData();
		}
		
		if(bUartNewData == TRUE){
			bUartNewData = FALSE;
			dataFrame_OK(&uart, RxBuf);
		}		
  }
	
	//HAL_Delay(100);
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSE, RCC_MCODIV_1);

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

	if (htim->Instance == htim3.Instance){
		//bDisplay = 1;
		
		static uint8_t iii = 0;
		if(iii++ >= 10){
			bDisplay = 1;
			HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);
			HAL_GPIO_TogglePin(LED2_GPIO_Port,LED2_Pin);
			iii=0;
			
			memcpy(TxBuf2,'\0',256);
			sprintf((char*)TxBuf2,"Vout1: %f, Vout2: %f, Vout3: %f, Vout4: %f\n", vout[0],vout[1],vout[2],vout[3]);
			HAL_UART_Transmit_DMA(&huart2,TxBuf2,strlen(TxBuf2));
			
			//HAL_UART_Transmit(&huart2,tempbuf,strlen(tempbuf),0xFFFF);
		}

	}

}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char * file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler_Debug */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
