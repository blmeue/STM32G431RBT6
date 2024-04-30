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

#include "lcd.h"
#include "led.h"
#include "timer.h"
#include "myusart.h"

#include "stdio.h"
#include "string.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

unsigned char ucled=0x00;
unsigned char lcd_text[30];
unsigned char usart_text[30];
unsigned char view=0;

__IO uint32_t uwTick_Lcd;
__IO uint32_t uwTick_Led;

unsigned char B1='1';
unsigned char B2='2';
unsigned char B3='3';

unsigned char Input_B1='@';
unsigned char Input_B2='@';
unsigned char Input_B3='@';

int F=1000;
int D=50;

unsigned char success_flag=0;//密码正确标志位
int lose_count=0;//输错次数

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
void usart_proc(void);
void led_proc(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_TIM4_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
    
    led_disp(ucled);
    
    LCD_Init();
    LCD_Clear(Black);
    LCD_SetBackColor(Black);
    LCD_SetTextColor(White);
    
    HAL_TIM_Base_Start_IT(&htim4);
    HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
    
    HAL_UART_Receive_IT(&huart1,&rxdat,1);
    
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
      if(rx_pointer!=0)
      {
          int temp=rx_pointer;
          HAL_Delay(2);
          if(temp==rx_pointer)
              usart_proc();
      } 
      key_proc();
      lcd_proc();
      led_proc();
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
    if((uwTick-uwTick_Lcd)<100)return;
    uwTick_Lcd=uwTick;
    
    switch(view)
    {
        case 0://密码输入界面
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"       PSD           ");
            LCD_DisplayStringLine(Line1,lcd_text);

            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"    B1:%c              ",Input_B1);
            LCD_DisplayStringLine(Line3,lcd_text);
       
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"    B2:%c              ",Input_B2);
            LCD_DisplayStringLine(Line4,lcd_text);
       
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"    B3:%c              ",Input_B3);
            LCD_DisplayStringLine(Line5,lcd_text);  
            break;
        
        case 1://输出状态界面
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"       STA           ");
            LCD_DisplayStringLine(Line1,lcd_text);

            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"    F:%dHz              ",F);
            LCD_DisplayStringLine(Line3,lcd_text);
       
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"    D:%d%%              ",D);
            LCD_DisplayStringLine(Line4,lcd_text);
            break;
    }
    
}

void key_proc(void)
{
    if(1==key[0].single_flag)//输入密码第一位
    {
        key[0].single_flag=0;
        if(0==view)//密码输入界面
        {
            if(Input_B1=='@')
            {
                Input_B1=0+'0';
            }
            else
            {
                Input_B1++;
                if(Input_B1==(10+'0'))
                    Input_B1=0+'0';
            }
        }
    }
    else if(1==key[1].single_flag)//输入密码第二位
    {
        key[1].single_flag=0;
        if(0==view)//密码输入界面
        {
            if(Input_B2=='@')
            {
                Input_B2=0+'0';
            }
            else
            {
                Input_B2++;
                if(Input_B2==(10+'0'))
                    Input_B2=0+'0';
            }
        }
    }
    else if(1==key[2].single_flag)//输入密码第三位
    {
        key[2].single_flag=0;
        if(0==view)//密码输入界面
        {
            if(Input_B3=='@')
            {
                Input_B3=0+'0';
            }
            else
            {
                Input_B3++;
                if(Input_B3==(10+'0'))
                    Input_B3=0+'0';
            }
        }
    }
    else if(1==key[3].single_flag)//确实密码
    {
        key[3].single_flag=0;
        if(0==view)//密码输入界面
        {
            //密码正确，调整到输出状态界面
            if((Input_B1==B1)&&(B2==Input_B2)&&(B3==Input_B3))
            {
                lose_count=0;
                view=1;
                success_flag=1;
                LCD_Clear(Black);
            }
            //密码错误，停留在密码输入界面，且显示界面的三位密码值重置为0
            else
            {
                lose_count++;
                Input_B1='@';
                Input_B2='@';
                Input_B3='@';
            }
        }
    }
}

void usart_proc(void)
{
     if(rx_pointer!=0)
     {
        if(7==rx_pointer){
             if((rxdata[0]==B1)&&(rxdata[1]==B2)&&(rxdata[2]==B3)&&(rxdata[3]=='-'))
             {
                 if((rxdata[4]>='0'&&rxdata[4]<='9')&&(rxdata[5]>='0'&&rxdata[5]<='9')&&(rxdata[6]>='0'&&rxdata[6]<='9'))
                 {
                     B1=rxdata[4];
                     B2=rxdata[5];
                     B3=rxdata[6];
                 }
             }
        }
         rx_pointer=0;
         memset(rxdata,0,sizeof(rxdata));
     }
}

void led_proc(void)
{
    if((uwTick-uwTick_Led)<50)return;
    uwTick_Led=uwTick;
    
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
