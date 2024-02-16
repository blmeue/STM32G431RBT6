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
#include "adc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "led.h"
#include "lcd.h"
#include "i2c_hal.h"
#include "timer.h"
#include "myusart.h"
#include "myadc.h"
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
void lcd_proc(void);
void key_proc(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

//lcd��
unsigned char ucled=0x00;

//����lcd��ʾ����Ҫ��ʾ������
unsigned char text[30];

//���ٺ���
__IO uint32_t uwTick_lcd;//����lcd��ʾ���ٶ�

//pa6\pa7��ռ�ձ�
unsigned char pa6_duty=10;
unsigned char pa7_duty=10;

//pb15\pb12�ĵ�ѹ
double pb15_v=0;//R37
double pb12_v=0;//R38


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
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM16_Init();
  MX_TIM17_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
    led_disp(ucled);
    
    LCD_Init();
    LCD_Clear(White);
    LCD_SetBackColor(White);
    LCD_SetTextColor(Blue2);
    
    //ʹ�ܶ�ʱ��4
    HAL_TIM_Base_Start_IT(&htim4);
    
    //�򿪶�ʱ��PWM���
    HAL_TIM_PWM_Start(&htim16,TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim17,TIM_CHANNEL_1);
    
    //�򿪶�ʱ�����벶��
    HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_1);
    HAL_TIM_IC_Start_IT(&htim3,TIM_CHANNEL_1);
    
    HAL_UART_Receive_IT(&huart1,&rxdat,1);
    
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
      key_proc();
    lcd_proc();
    /* USER CODE BEGIN 3 */
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

void lcd_proc(void)
{
    if((uwTick-uwTick_lcd)<200)return;
    uwTick_lcd=uwTick;
    //�ڶ�����ʾpa6��ռ�ձ�
    sprintf((char *)text,"pa6_duty:%d %%",pa6_duty);
    LCD_DisplayStringLine(Line1,text);
    //��������ʾpa7��ռ�ձ�
    sprintf((char *)text,"pa7_duty:%d %%",pa7_duty);
    LCD_DisplayStringLine(Line2,text);
    //��������ʾR39(tim3)��Ƶ��frq2;
    sprintf((char *)text,"tim3_frq2:%d HZ",frq2);
    LCD_DisplayStringLine(Line3,text);    
    //��������ʾR39(tim3)��ռ�ձ�duty2��
    sprintf((char *)text,"tim3_duty2:%0.3f %%",duty2);
    LCD_DisplayStringLine(Line4,text);     
    //��������ʾR40(tim2)��Ƶ��frq1;
    sprintf((char *)text,"tim2_frq1:%d HZ",frq1);
    LCD_DisplayStringLine(Line5,text);     
    //��������ʾR40(tim2)��ռ�ձ�duty1;
    sprintf((char *)text,"tim1_duty1:%0.3f %%",duty1);
    LCD_DisplayStringLine(Line6,text);  
    //�ڰ��ж����洢��eeprom�е�Ƶ��
    unsigned int eeprom_temp=(eeprom_read(1)<<8)+eeprom_read(2);
    sprintf((char *)text,"eeprom_frq:%d HZ",eeprom_temp);
    LCD_DisplayStringLine(Line7,text);         
    //�ھ��ж���pb15(R37)�ĵ�ѹ
     sprintf((char *)text,"pb15_v:%0.2f V",pb15_v);
    LCD_DisplayStringLine(Line8,text);   
    //��ʮ�ж���pb12(R38)�ĵ�ѹ
     sprintf((char *)text,"pb12_v:%0.3f V",pb12_v);
    LCD_DisplayStringLine(Line9,text);
}

void key_proc(void)
{
    if(1==key[0].single_flag)
    {
     sprintf((char *)text,"%s","key1down");
     LCD_DisplayStringLine(Line0,text);
        ucled=ucled|0x01;
        key[0].single_flag=0;
    }
    else if(1==key[1].single_flag)//��tim2��Ƶ�ʱ��浽eeprom
    {
     sprintf((char *)text,"%s","key2down");
     LCD_DisplayStringLine(Line0,text);
        ucled=ucled&0xfe;
        unsigned char frq_h;
        unsigned char frq_l;
        frq_h=frq1>>8;
        frq_l=(frq1&0xff);
        eeprom_write(1,frq_h);
        HAL_Delay(10);//ʮ����
        eeprom_write(2,frq_l);
        key[1].single_flag=0;
    }
    else if(1==key[2].single_flag)//pa6��ռ�ձ�����
    {
        sprintf((char *)text,"%s","key3down");
        LCD_DisplayStringLine(Line0,text);
        ucled=ucled|0x02;
        pa6_duty+=10;
        if(pa6_duty>=100)pa6_duty=10;
        __HAL_TIM_SetCompare(&htim16,TIM_CHANNEL_1,pa6_duty);
        key[2].single_flag=0;
    }
    else if(1==key[3].single_flag)//pa7��ռ�ձ�����
    {
        sprintf((char *)text,"%s","key3down");
        LCD_DisplayStringLine(Line0,text);
        ucled=ucled&0xfd;
         pa7_duty+=10;
        if(pa7_duty>=100)pa7_duty=10;
        __HAL_TIM_SetCompare(&htim17,TIM_CHANNEL_1,pa6_duty);
        key[3].single_flag=0;
    }
    pb12_v=get_adc(&hadc1);//pb12
    pb15_v=get_adc(&hadc2);//pb15
    led_disp(ucled);
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
