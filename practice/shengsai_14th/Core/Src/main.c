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

#include "lcd.h"
#include "led.h"
#include "timer.h"
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
void key_proc(void);
void lcd_proc(void);
void led_proc(void);


/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

//led灯
unsigned char ucled;

//在lcd屏上显示的数据
unsigned char lcd_text[30];


//显示界面选择
unsigned char view=0;
__IO uint32_t uwTick_lcd;//减速函数
__IO uint32_t uwTick_led;//减速函数

//参数调整选择
unsigned char choose_RK=0;


unsigned char flag_k2=0;

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
  MX_TIM4_Init();
  MX_TIM3_Init();
  MX_TIM16_Init();
  /* USER CODE BEGIN 2 */

    led_disp(0x00);
    
    LCD_Init();
    LCD_Clear(Black);
    LCD_SetBackColor(Black);
    LCD_SetTextColor(White);
    
    HAL_TIM_Base_Start_IT(&htim4);
    
    HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);

    HAL_TIM_IC_Start_IT(&htim3,TIM_CHANNEL_2);

/* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

      key_proc();
      led_proc();
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
    if(1==key[0].single_flag)//B1被按下
    {
        key[0].single_flag=0;
        
        view++;
        if(3==view)view=0;
        LCD_Clear(Black);
        
    }
    else if(1==key[1].single_flag)//B2被按下
    {
        key[1].single_flag=0;

        if(0==view&&flag_k2==0)//数据界面,选择低频或高频模式，按键按下后，5秒内不可再触发切换功能
        {
            if(0==uwTick_view)
            flag_k2=1;
        }
        else if(1==view)//参数界面
        {
            choose_RK=!choose_RK;
        }
        
    }
    else if(1==key[2].single_flag)//B3被按下
    {
        key[2].single_flag=0;
        if(1==view)//参数界面，当前可调整参数加1
        {
            if(0==choose_RK)//可调整参数为R
            {
                R++;
                if(R>10)R=1;
            }
            else if(1==choose_RK)//可调整参数为K
            {
                K++;
                if(K>10)K=1;
            }
        }
    }
    else if(1==key[3].single_flag)//B4被按下
    {
        key[3].single_flag=0;
        if(1==view)//参数界面，当前可调整参数减1
        {
            if(0==choose_RK)//可调整参数为R
            {
                R--;
                if(R<=0)R=10;
            }
            else if(1==choose_RK)//可调整参数为K
            {
                K--;
                if(K<=0)K=10;
            }            
        }
        else if(0==view)//数据界面
        {
            
            if(1==key[3].long_single)
            {
                lock=1;//进入锁定模式
                key[3].long_single=0;
            }
            else 
            {
                if(lock==1)
                {
                    lock=0;//实现解锁
                }
            }
        }
    }
    

}


void lcd_proc(void)
{
   if((uwTick-uwTick_lcd)<=100)return;
        uwTick_lcd=uwTick;   
    
//   if((200==pa1_frq)&&(speed_v>L_max))L_max=speed_v;    
//    if((100==pa1_frq)&&(speed_v>H_max))H_max=speed_v;
    
    switch(view)
    {
        case 0://数据显示界面
            choose_RK=0;//数据界面进入到参数界面，默认可调整的参数为R
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"        %s","DATA");
            LCD_DisplayStringLine(Line1,lcd_text);
        
            if(200==pa1_frq)//低频模式  //频率完全改变完后改变M的值
            {
                memset(lcd_text,0,sizeof(lcd_text));
                sprintf((char *)lcd_text,"     %s             ","M=L");
                LCD_DisplayStringLine(Line3,lcd_text);
                
            }
            else if(100==pa1_frq)//高频模式
            {
                memset(lcd_text,0,sizeof(lcd_text));
                sprintf((char *)lcd_text,"     %s             ","M=H");
                LCD_DisplayStringLine(Line3,lcd_text);   
            }
            
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     P=%d%%             ",pa1_duty);
            LCD_DisplayStringLine(Line4,lcd_text);
            
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     V=%0.1f             ",speed_v);
            LCD_DisplayStringLine(Line5,lcd_text);    
            
            break;
        case 1://参数界面
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"        %s             ","PARA");
            LCD_DisplayStringLine(Line1,lcd_text);    
 
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     R=%02d             ",R);
            LCD_DisplayStringLine(Line3,lcd_text);    
        
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     K=%02d             ",K);
            LCD_DisplayStringLine(Line4,lcd_text);            
            break;
        case 2://记录界面
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"        %s             ","RECD");
            LCD_DisplayStringLine(Line1,lcd_text);    

            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     N=%d             ",pwm_n);
            LCD_DisplayStringLine(Line3,lcd_text);  
        
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     MH=%0.1f             ",H_max);
            LCD_DisplayStringLine(Line4,lcd_text);
            
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     ML=%0.1f             ",L_max);
            LCD_DisplayStringLine(Line5,lcd_text);         
            break;
    }
}

void led_proc(void)
{
    if((uwTick-uwTick_led)<100)return;
    uwTick_led=uwTick;
    
    //数据界面：LED1亮
    if(0==view)
    {
        ucled=ucled|0x01;
    }
    else
    {
        ucled=ucled&0xef;
    }
    

    //锁定状态：LED3亮
    if(1==lock)
    {
        ucled|=0x04;
    }
    else
    {
        ucled=ucled&0xfb;
    }
 
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
