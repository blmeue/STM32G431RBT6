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
#include "myusart.h"
#include "timer.h"
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

unsigned char ucled=0x00;//led��
unsigned char text[30];//����lcd��ʾ����Ҫ��ʾ������

//����ͨ����ռ�ձ�
unsigned char pa6_duty=10;
unsigned char pa7_duty=10;


//���ٺ���
__IO uint32_t uwTick_Lcd =0;//����Lcd_Pric ��ִ���ٶ�


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
    
    led_disp(0x00);
    
    LCD_Init();
    LCD_Clear(White);
    LCD_SetBackColor(White);
    LCD_SetTextColor(Blue2);

    HAL_UART_Receive_IT(&huart1,&rxdat,1);
    
    HAL_TIM_Base_Start_IT(&htim4);//ʹ�ܶ�ʱ��4
    
    //PWM��
    HAL_TIM_PWM_Start(&htim16,TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim16,TIM_CHANNEL_1);

    //�����벶��Ķ�ʱ��
    //������ʱ�����벶���ж�
    HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_1);//Ƶ�ʲ�������ʱ������
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
    
    //����1������
   if(1==key[0].single_flag) 
   {
       sprintf((char *)text,"     %s","key1down");
       LCD_DisplayStringLine(Line0,text);
       pa6_duty+=10;
       if(pa6_duty>=100)pa6_duty=10;
   __HAL_TIM_SetCompare(&htim16,TIM_CHANNEL_1,pa6_duty);//����ռ�ձ�

       key[0].single_flag=0;

   }
    //����2������
   if(1==key[1].single_flag) 
   {
       sprintf((char *)text,"     %s","key2down");
       LCD_DisplayStringLine(Line0,text);
              pa7_duty+=10;
       if(pa7_duty>=100)pa7_duty=10;
   __HAL_TIM_SetCompare(&htim16,TIM_CHANNEL_1,pa6_duty);//����ռ�ձ�

       key[1].single_flag=0;
   }
    //����3������
   if(1==key[2].single_flag) 
   {
       sprintf((char *)text,"     %s","key3down");
       LCD_DisplayStringLine(Line0,text);
       pa6_duty-=10;
       if(pa6_duty<10)pa6_duty=90;
   __HAL_TIM_SetCompare(&htim17,TIM_CHANNEL_1,pa7_duty);//����ռ�ձ�
 
       key[2].single_flag=0;
   }
   //����4������
   if(1==key[3].single_flag) 
   {
       sprintf((char *)text,"     %s","key4down");
       LCD_DisplayStringLine(Line0,text);
       pa7_duty-=10;
       if(pa7_duty<10)pa7_duty=90;
   __HAL_TIM_SetCompare(&htim17,TIM_CHANNEL_1,pa7_duty);//����ռ�ձ�

       key[3].single_flag=0;
   }
   


   led_disp(ucled);
}

void lcd_proc(void)
{
   	if((uwTick - uwTick_Lcd) < 200) return; //���ٺ���//300����ˢ��һ��
		uwTick_Lcd = uwTick; 
       //�ڶ�����ʾpa6��ռ�ձ�
   sprintf((char *)text,"pa6:%d%%",pa6_duty);
       LCD_DisplayStringLine(Line1,text);
   //��������ʾpa7��ռ�ձ�
   sprintf((char *)text,"pa7:%d%%",pa7_duty);
       LCD_DisplayStringLine(Line2,text);

   //��������ʾ��ʱ��2��Ƶ��,R40
   //LCD_ClearLine(Line3);
   sprintf((char *)text,"tim2_frq:%05d Hz",frq1);
       LCD_DisplayStringLine(Line3,text);
   //��������ʾ��ʱ��3��Ƶ��,R39
   //LCD_ClearLine(Line4);
   sprintf((char *)text,"tim3_frq:%05d Hz",frq2);
   LCD_DisplayStringLine(Line4,text);
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
