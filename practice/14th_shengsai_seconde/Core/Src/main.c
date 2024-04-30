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
#include "timer.h"
#include "lcd.h"
#include "myadc.h"

#include "math.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

unsigned char ucled=0x00;
unsigned char lcd_text[30];
unsigned char view=0;

__IO uint32_t uwTick_led;
__IO uint32_t uwTick_lcd;
__IO uint32_t uwTick_V;


double v_speed;
unsigned char M=0;
unsigned int pa1_duty;

unsigned char choose_mode=0;

unsigned int R=1;
unsigned int K=1;

unsigned int R_change=1;
unsigned int K_change=1;

unsigned int N=0;
double MH=0;//高频模式最大速度
double ML=0;//高频模式最小速度

unsigned char locking;
unsigned char choose_RK=0;

unsigned int pa1_frq=4000;

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
void PA1_Change(void);

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

    LCD_Init();
    LCD_Clear(Black);
    LCD_SetBackColor(Black);
    LCD_SetTextColor(White);
    
    led_disp(ucled);
    
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
      PA1_Change();
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

void PA1_Change(void)
{    
    if(0==locking)//不处于锁定状态，占空比受R37电位器输出电压控制
    {
        double R_37;
        R_37=GetAdc(&hadc2);
        if(R_37<=1)
        {
            pa1_duty=10;
          __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,pa1_duty);  
        }
        else if(R_37>=3)
        {
            pa1_duty=85;
          __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,pa1_duty);  
        }
        else
        {
            pa1_duty=((75.0/2)*R_37-(55.0/2.0));
          __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,pa1_duty);  
           
        }
    }    
}



void lcd_proc(void)
{
    if((uwTick-uwTick_lcd)<100)return;
    uwTick_lcd=uwTick;
    
    switch(view)
    {
        case 0://数据显示界面
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"        DATA          ");
            LCD_DisplayStringLine(Line1,lcd_text);
        
            memset(lcd_text,0,sizeof(lcd_text));
            switch(M)
            {
                case 0://低频
                    sprintf((char *)lcd_text,"     M=L          ");
                    break;
                case 1://高频
                    sprintf((char *)lcd_text,"     M=H          ");
                    break;
            }
            LCD_DisplayStringLine(Line3,lcd_text);
            
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     P=%d%%               ",pa1_duty);
            LCD_DisplayStringLine(Line4,lcd_text);
            
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     V=%0.1f              ",v_speed);
            LCD_DisplayStringLine(Line5,lcd_text);
            
            break;
        case 1://参数界面
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"        PARA          ");
            LCD_DisplayStringLine(Line1,lcd_text);
        
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     R=%d               ",R_change);
            LCD_DisplayStringLine(Line3,lcd_text);
        
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     K=%d               ",K_change);
            LCD_DisplayStringLine(Line4,lcd_text);
        
            break;
        case 2://统计界面
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"        RECD          ");
            LCD_DisplayStringLine(Line1,lcd_text);
        
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     N=%d               ",N);
            LCD_DisplayStringLine(Line3,lcd_text);
        
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     MH=%0.1f              ",MH);
            LCD_DisplayStringLine(Line4,lcd_text);
        
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     ML=%0.1f              ",ML);
            LCD_DisplayStringLine(Line5,lcd_text);
        
            break;
    }
    

    
}

void key_proc(void)
{
    if(1==key[0].single_flag)//界面切换按键
    {
        key[0].single_flag=0;
        if(0==view)//每次从数据界面进入到参数界面时，默认当前可调整的参数为R
        {
            choose_RK=0;
        }
        if(1==view)//从参数界面退出时，新的R参数和K参数生效
        {
            R=R_change;
            K=K_change;
        }
        view++;
        if(3==view)view=0;
        LCD_Clear(Black);
    }
    else if(1==key[1].single_flag)//选择按键
    {
        key[1].single_flag=0;
        if(0==view)//选择切换低频或高频模式，按键按下后5S内不能再次触发切换功能
        { 
               if(0==choose_mode)
               {
                   choose_mode=1;//按键按下标志位               
               }                                
        }
        else if(1==view)//切换选择R或K参数，每次从数据界面进入参数界面，默认可调整的参数为R，从参数界面退出时，新的R参数和K参数生效
        {
            choose_RK=!choose_RK;
        }
    }
    else if(1==key[2].single_flag)//参数加按键
    {
        key[2].single_flag=0;
        if(1==view)
        {
            if(0==choose_RK)//可调整参数为R
            {
                R_change++;
                if(R_change==11)R_change=1;
            }
            else if(1==choose_RK)//可调整参数为K
            {
                K_change++;
                if(K_change==11)K_change=1;
            }
        }
    }
    else if(1==key[3].single_flag)//参数减按键\解锁
    {
        key[3].single_flag=0;
        if(0==view)//解锁
        {
            if(1==locking)
            locking=0;
        }
        else if(1==view)//参数减1
        {
           if(0==choose_RK)//可调整参数为R
            {
                R_change--;
                if(R_change==0)R_change=10;
            }
            else if(1==choose_RK)//可调整参数为K
            {
                K_change--;
                if(K_change==0)K_change=10;
            } 
        }
    }
    else if(1==key[3].long_flag)//锁定
    {
        key[3].long_flag=0;
        if(0==view)
        {
            locking=1;//锁定
        }
    }
}


void led_proc(void)
{
    if((uwTick-uwTick_led)<50)return;
    uwTick_led=uwTick;
    
    if(0==view)//数据界面，LD1亮
    {
        ucled|=0x01;
    }
    else
    {
        ucled&=0xfe;
    }
    
    if(1==locking)//锁定状态,LD3亮
    {
        ucled|=0x04;
    }
    else
    {
        ucled&=0xfb;
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
