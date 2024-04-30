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
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "stdio.h"
#include "string.h"

#include "led.h"
#include "lcd.h"
#include "myadc.h"
#include "timer.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

unsigned char ucled=0x00;
unsigned char lcd_text[30];
unsigned char view=0;

__IO uint32_t uwTick_Led;
__IO uint32_t uwTick_Lcd;

double R37_V;
unsigned int R=4;

unsigned char LED_Flag=0;

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
void led_proc(void);
void PA7_change(void);


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
  MX_ADC2_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */

    led_disp(ucled);
    
    LCD_Init();
    LCD_Clear(Black);
    LCD_SetBackColor(Black);
    LCD_SetTextColor(White);
    
    HAL_TIM_Base_Start_IT(&htim4);
    
    HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2);
    
    HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_2);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
      key_proc();
      lcd_proc();
      PA7_change();
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
        case 0://数据显示界面
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"       Data          ");
            LCD_DisplayStringLine(Line1,lcd_text);
        
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"   FRQ:%dHz          ",pa1_frq);
            LCD_DisplayStringLine(Line3,lcd_text);
        
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"   R37:%0.2fV            ",R37_V);
            LCD_DisplayStringLine(Line5,lcd_text);
        
            break;
        case 1://参数设置界面
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"       Para          ");
            LCD_DisplayStringLine(Line1,lcd_text);
        
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"      R:%d          ",R);
            LCD_DisplayStringLine(Line3,lcd_text);
 
            break;
    }
    
}

void key_proc(void)
{
    if(1==key[0].single_flag)//界面切换
    {
        key[0].single_flag=0;
        view=!view;
        LCD_Clear(Black);
    }
    else if(1==key[1].single_flag)//R增加2
    {
        key[1].single_flag=0;
        R+=2;
        if(R==12)R=2;
    }
    else if(1==key[2].single_flag)//R减少2
    {
        key[2].single_flag=0;
        R-=2;
        if(R==0)R=10;
    }
    else if(1==key[3].single_flag)//启用或禁用LED灯
    {
        key[3].single_flag=0;
        LED_Flag=!LED_Flag;
    }
}

void PA7_change(void)
{
    R37_V=Get_Adc(&hadc2);
    
    //频率=（80MHZ）/（重装载值+1）/（预分频系数+1）
    unsigned int frq;
    frq=80000000/100/(pa1_frq/R)-1;
    //预分频系数=（80000000/100/频率）-1
    
    __HAL_TIM_SET_PRESCALER(&htim3,frq);
    
    //占空比=（（比较值）/重装载值）*100
    double duty;
    duty=(100/3.3)*R37_V;
    __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_2,duty);
}

void led_proc(void)
{
    if((uwTick-uwTick_Led)<50)return;
    uwTick_Led=uwTick;
    //LED指示灯功能启用
    if(0==LED_Flag)
    {
        if(0==view)
        {
            ucled|=0x01;
            ucled&=0xfd;
        }
        else if(1==view)
        {
            ucled|=0x02;
            ucled&=0xfe;
        }
        
        if((R37_V<1.0)||(R37_V>=3.0))
        {
            ucled&=0xfb;
        }
        else
        {
            ucled|=0x04;
        }
        
        if((pa1_frq<1000)||(pa1_frq>5000))
        {
            ucled&=0xf7;
        }
        else
        {
            ucled|=0x08;
        }
        
        led_disp(ucled);
        
    }
    //禁用LED指示灯功能
    else
    {
        led_disp(0x00);
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
