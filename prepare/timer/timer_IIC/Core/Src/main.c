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
#include "lcd.h"
#include "timer.h"
#include "myusart.h"
#include "i2c_hal.h"
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
void lcd_proc(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

unsigned char text[30];//保存lcd所要显示的数据
unsigned char ucled=0x00;//led灯

//减速函数，控制lcd_proc的速度
__IO uint32_t uwTick_pcd=0;

//两个通道(pa6,pa7)的占空比
unsigned char pa6_duty=10;
unsigned char pa7_duty=10;

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
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM16_Init();
  MX_TIM17_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

    led_disp(0X00);
    LCD_Init();
    LCD_Clear(White);
    LCD_SetBackColor(White);
    LCD_SetTextColor(Blue2);
    
    HAL_UART_Receive_IT(&huart1,&rxdat,1);//打开串口接收中断
    
    HAL_TIM_Base_Start_IT(&htim4);//使能定时器4
    
    //打开定时器PWM输出
    HAL_TIM_PWM_Start(&htim16,TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim17,TIM_CHANNEL_1);
    
    //打开定时器输入捕获
    HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_1);
    HAL_TIM_IC_Start_IT(&htim3,TIM_CHANNEL_1);
   
   
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
      key_proc();
      lcd_proc();
      
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
    if(1==key[0].single_flag)
    {
        ucled=ucled|0x01;
        sprintf((char *)text,"    %s","key1down");
        LCD_DisplayStringLine(Line0 ,text);
        key[0].single_flag=0;
    }
    else if(1==key[1].single_flag)
    {
        ucled=ucled&0xfe;
        //将频率(16位)存到eeprom中
        //将频率分为高八位和低八位，分别存到eeprom中
        unsigned char frq1_h=frq1>>8;
        unsigned char frq1_l=frq1&0xff;
        eeprom_write(1,frq1_h);
        HAL_Delay(10);//十毫秒
        eeprom_write(2,frq1_l);
       sprintf((char *)text,"    %s","key2down");
        LCD_DisplayStringLine(Line0 ,text);
        key[1].single_flag=0;
    }
    else if(1==key[2].single_flag)
    {
        ucled=ucled|0x02;
        sprintf((char *)text,"    %s","key3down");
        LCD_DisplayStringLine(Line0 ,text);
        //pa6_duty增加
        pa6_duty+=10;
        if(pa6_duty>=100)pa6_duty=10;
        __HAL_TIM_SetCompare(&htim16,TIM_CHANNEL_1,pa6_duty);
        key[2].single_flag=0;
    }
    else if(1==key[3].single_flag)
    {
        ucled=ucled&0xdf;
        sprintf((char *)text,"    %s","key4down");
        LCD_DisplayStringLine(Line0 ,text);
        //pa7_duty增加
        pa7_duty+=10;
        if(pa7_duty>=100)pa7_duty=10;
        __HAL_TIM_SetCompare(&htim17,TIM_CHANNEL_1,pa7_duty);

       key[3].single_flag=0;
    }
    
    led_disp(ucled);
}

void lcd_proc(void)
{
    if((uwTick-uwTick_pcd)<200)return;//减速函数
    uwTick_pcd=uwTick;
    
  //第二行显示pa6的占空比
    sprintf((char *)text,"pa6_duty:%d%%",pa6_duty);
    LCD_DisplayStringLine(Line1,text);
    
  //第三行显示pa7的占空比
    sprintf((char *)text,"pa7_duty:%d%%",pa7_duty);
    LCD_DisplayStringLine(Line2,text);
    
  //第四行显示R39(tim3)的频率frq2;
 //   sprintf((char *)text,"tim3_frq2:%05d Hz",frq2);
 //   LCD_DisplayStringLine(Line3,text);
    sprintf((char *)text,"tim2_frq:%05d Hz",frq2);
       LCD_DisplayStringLine(Line3,text);   
  //第五行显示R39(tim3)的占空比duty2; 
    sprintf((char *)text,"tim3_duty2:%0.3f%%",duty2);
    LCD_DisplayStringLine(Line4,text);    
    
  //第六行显示R40(tim2)的频率frq1;
    sprintf((char *)text,"tim2_frq1:%05d Hz",frq1);
    LCD_DisplayStringLine(Line5,text);
    
  //第七行显示R40(tim2)的占空比duty1; 
    sprintf((char *)text,"tim2_duty1:%0.3f%%",duty1);
    LCD_DisplayStringLine(Line6,text);   
    
  //读出存储在eeprom中的频率
    unsigned int eeprom_temp=(eeprom_read(1)<<8)+(eeprom_read(2));
    sprintf((char *)text,"eeprom_frq1:%05d Hz",eeprom_temp);
    LCD_DisplayStringLine(Line7,text);

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
