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
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "led.h"
#include "timer.h"
#include "lcd.h"
#include "myusart.h"
#include "stdio.h"
#include "String.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

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
void key_proc(void);
void usart_proc(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
unsigned char text[30];//在LCD上显示的文字
unsigned char ucled=0x00;//led灯

//定义两个通道的占空比

unsigned char pa6_duty;
unsigned char pa7_duty;

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
  MX_TIM2_Init();
  MX_TIM16_Init();
  MX_TIM17_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
    led_disp(0x00);
    LCD_Init();
    LCD_Clear(White);
    LCD_SetBackColor(White);
    LCD_SetTextColor(Blue2);
    HAL_UART_Receive_IT(&huart1,&rxdat,1);
    HAL_TIM_Base_Start_IT(&htim2);
   
   /*
   PA6输出频率100MHZ，占空比可调
   PA7输出频率200MHZ，占空比可调
   */
   
   HAL_TIM_PWM_Start(&htim16,TIM_CHANNEL_1);//PWM输出打开
   HAL_TIM_PWM_Start(&htim17,TIM_CHANNEL_1);//PWM输出打开
   
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
      key_proc();
      if(rxpointer!=0)
      {
          int temp=rxpointer;
          HAL_Delay(1);
          if(temp==rxpointer)
             usart_proc(); 
          
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

void key_proc(void)
{
    char str[20];
    if(1==key[0].single_flag)
    {
        //led1亮
        ucled=ucled|(0x01);
        sprintf((char *)text,"%s","key1down");
        LCD_DisplayStringLine(Line4,text);

        sprintf(str,"%s\r\n","key1down");
        key[0].single_flag=0;
        HAL_UART_Transmit(&huart1,(uint8_t *)str,strlen(str),50);
    }

    if(1==key[1].single_flag)//PA6占空比增加10%
    {
        //led1灭
        ucled=ucled&(0xfe);
        sprintf((char *)text,"%s","key2down");
         LCD_DisplayStringLine(Line4,text);
        
        pa6_duty+=10;
        if(pa6_duty>=100)pa6_duty=10;
        sprintf((char *)text,"PA6:%d",pa6_duty);
        LCD_DisplayStringLine(Line5,text);
        //将占空比应用数输出中，即设置PWM波的占空比
        //第一个参数是哪个定时器，第二个参数是第几个通道，第三次参数是设置的占空比值
        __HAL_TIM_SetCompare(&htim16,TIM_CHANNEL_1,pa6_duty);
         key[1].single_flag=0;
    }
    
    if(1==key[2].single_flag)
    {
         //led2亮
        ucled=ucled|(0x02);
          sprintf((char *)text,"%s","key3down");
         LCD_DisplayStringLine(Line4,text);
        pa7_duty+=10;
        if(pa7_duty>=100)pa7_duty=10;
        sprintf((char *)text,"PA7:%d",pa7_duty);
        LCD_DisplayStringLine(Line5,text);
        //将占空比应用数输出中，即设置PWM波的占空比
        //第一个参数是哪个定时器，第二个参数是第几个通道，第三次参数是设置的占空比值
        __HAL_TIM_SetCompare(&htim17,TIM_CHANNEL_1,pa7_duty);
         key[2].single_flag=0;
  }
    if(1==key[3].single_flag)
    {
         //led2灭   
         ucled=ucled&(0xfd);
         sprintf((char *)text,"%s","key4down");
         LCD_DisplayStringLine(Line4,text);
         key[3].single_flag=0;
    }
    led_disp(ucled);
}

//从电脑的串口助手通过串口发送消息给板子
//并返回到串口助手
void usart_proc(void)
{
    if(rxpointer!=0)//判断是否有接收到字符
    {
        rxdata[rxpointer++]='\r';
        rxdata[rxpointer++]='\n';
        
        HAL_UART_Transmit(&huart1,(uint8_t *)rxdata,strlen(rxdata),50);//将串口助手输入的字符重新发送到串口助手
        memset(rxdata,0,strlen(rxdata));
        rxpointer=0;
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
