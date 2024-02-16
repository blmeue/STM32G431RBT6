/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "key.h"
#include "lcd.h"
#include "led.h"
#include "myusart.h"
#include "stdio.h"
#include "string.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

//减速函数变量
__IO uint32_t uwTick_Led = 0;//控制Led_Proc的执行速度
__IO uint32_t uwTick_Lcd = 0;//控制Lcd_Proc的执行速度
__IO uint32_t uwTick_Key = 0;//控制Key_Proc的执行速度
__IO uint32_t uwTick_Usart = 0;//控制Usart_Proc的执行速度


//key的专用变量
unsigned char keynum;
unsigned char key_down;
unsigned char key_old;
unsigned char key_up;

// lcd的专用变量

// led的专用变量
unsigned char dsp;

// myusart的专用变量


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void Led_Proc(void);
void Key_Proc(void);
void Lcd_proc(void);
void usart_rx_proc(void);

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

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
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  LCD_Init();
  led_disp(0x00);
  LCD_Clear(White);
  LCD_SetTextColor(Blue2);
  LCD_SetBackColor(White);
  
  HAL_UART_Receive_IT(&huart1,&rxdat,1);//初始化串口，每次只能接收一个字符
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
      Key_Proc();
      Led_Proc();
      //防止接收不完整
      if(rx_pointer!=0){
          int temp=rx_pointer;
          HAL_Delay(1);
          if(temp==rx_pointer)usart_rx_proc();
      }
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV3;
  RCC_OscInitStruct.PLL.PLLN = 20;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void Led_Proc(void)
{
    if((uwTick-uwTick_Led)<20)return;
    
    uwTick_Led=uwTick;
    led_disp(dsp);
}

void Key_Proc(void)
{
    if((uwTick-uwTick_Key)<50)return;
    
    uwTick_Key=uwTick; 
    
    keynum=key_scan();
    key_down=keynum &(key_old^keynum);//ucKey_Old ^ ucKey_Val：相同为0，不同为1
    key_up=keynum &(key_old^keynum);    
    key_old=keynum;
    
    if(key_down==1)
    {
        dsp=0x55;
    }
    if(key_down==2)
    {
      dsp=0xaa;  
    }
    if(key_down==3)
    {
        dsp=0x00;
    }
    if(key_down==4)
    {
        Lcd_proc();
    }
}

void Lcd_proc(void)
{
  if((uwTick-uwTick_Lcd)<300)return;
    uwTick_Lcd=uwTick; 

   //在屏幕显示Hello,world;
    
    LCD_DisplayStringLine(Line4,(unsigned char *)"Hello,world!");
}    

//单片机通过串口发送消息到电脑上
//void usart_tx_proc(void)
//{
//    
//}

//接收函数：单片机通过串口接收从电脑（里的串口助手输入数据）发送过来的数据
//并将接收到的数据重新发送到电脑上
void usart_rx_proc(void)
{
    if((uwTick-uwTick_Usart)<100)return;
    uwTick_Usart=uwTick;   
    if(rx_pointer!=0){
    char str[30];
    sprintf(str,"%s \r\n",rxdata);
    HAL_UART_Transmit(&huart1,(unsigned char *)str,sizeof(str),50);    //将数据发送到电脑（串口助手）上
   // HAL_UART_Transmit(&huart1,(unsigned char *)rxdata,sizeof(rxdata),50);       
    rx_pointer=0;
    memset(rxdata,0,30);
    memset(str,0,30);
    }
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
