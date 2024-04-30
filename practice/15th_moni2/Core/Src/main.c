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

#include "stdio.h"
#include "string.h"
#include "lcd.h"
#include "led.h"
#include "timer.h"
#include "myadc.h"
#include "myusart.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

__IO uint32_t uwTick_Lcd;
__IO uint32_t uwTick_Led;

unsigned char view=0;
unsigned char lcd_text[30];
unsigned char usart_text[30];

unsigned char ucled=0x00;

double temp;//温度
double R37_v;
unsigned char mode=0;//当前控制模式
unsigned char gear=1;//档位

unsigned char usart_flag=0;
unsigned char usart_led=0;


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
void Lcd_proc(void);
void Key_proc(void);
void usart_proc(void);
void led_proc(void);
void pwm_proc(void);
void temp_proc(void);

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
  MX_TIM4_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  
    led_disp(ucled);
    
    LCD_Init();
    LCD_Clear(Black);
    LCD_SetBackColor(Black);
    LCD_SetTextColor(White);
    
    HAL_TIM_Base_Start_IT(&htim4);
    
    HAL_UART_Receive_IT(&huart1,&rxdat,1);
    
    HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
    
    
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
          if(rx_pointer==temp)
              usart_proc();
      }  
      
      Key_proc();
      temp_proc();
      Lcd_proc();
      led_proc();
      pwm_proc();
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

void temp_proc(void)
{
    R37_v=Get_Adc(&hadc2);
    if(R37_v<=1)
        temp=20;
    else if(R37_v>=3)
        temp=40;
    else
    {
        temp=10*R37_v+10;
    }
    if(0==mode)//自动档
    {
        if(temp<25)
            gear=1;
        else if((temp>=25)&&(temp<=30))
            gear=2;
        else if(temp>30)
            gear=3;
    }
}

void Lcd_proc(void)
{
    if((uwTick-uwTick_Lcd)<100)return;
    uwTick_Lcd=uwTick;
    
    switch(view)
    {
        case 0://数据界面
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"        DATA          ");
            LCD_DisplayStringLine(Line1,lcd_text);
        
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     TEMP:%0.1f        ",temp);
            LCD_DisplayStringLine(Line3,lcd_text);
        
            memset(lcd_text,0,sizeof(lcd_text));
            if(0==mode)//自动挡模式
            {
                sprintf((char *)lcd_text,"     MODE:Auto         ");
            }
            else if(1==mode)//手动档模式
            {
                sprintf((char *)lcd_text,"     MODE:Manu         ");
 
            }
            LCD_DisplayStringLine(Line4,lcd_text);
            
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     GEAR:%d        ",gear);
            LCD_DisplayStringLine(Line5,lcd_text);
            
            break;
        case 1://睡眠界面
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     SLEEPING          ");
            LCD_DisplayStringLine(Line4,lcd_text);

            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     TEMP:%0.1f          ",temp);
            LCD_DisplayStringLine(Line5,lcd_text);
                
            break;
    }
}

void Key_proc(void)
{
    
    if((1==key[0].single_flag)||(1==usart_flag))
    {
        if(1==key[0].single_flag)
            key[0].single_flag=0;
        if(1==usart_flag)
            usart_flag=0;
        
       uwTick_sleep=0;
        if(0==view){
        
            mode=!mode;
        }
        else if(1==view)//处于睡眠界面下时，唤醒数据界面
        {
            view=0;
            LCD_Clear(Black);
        }
    }
    else if((1==key[1].single_flag)||(2==usart_flag))
    {
        if(1==key[1].single_flag)
        key[1].single_flag=0;
        if(2==usart_flag)
            usart_flag=0;
        
        uwTick_sleep=0;
        if((1==mode)&&(0==view))//手动控制模式下,且不处于睡眠界面下
        {
            gear++;
            if(4==gear)
            {
                gear=1;
            }
        }
        else if(1==view)//处于睡眠界面下时，唤醒数据界面
        {
            view=0;
            LCD_Clear(Black);
        }
    }
    else if((1==key[2].single_flag)||(3==usart_flag))
    {
        if(1==key[2].single_flag)
        key[2].single_flag=0;
        if(3==usart_flag)
            usart_flag=0;
        
        uwTick_sleep=0;
        
        if((1==mode)&&(0==view))//手动控制模式下,且不处于睡眠界面下
        {
            gear--;
            
            if(0==gear)
            {
                gear=3;
            }
        }
        else if(1==view)//处于睡眠界面下时，唤醒数据界面
        {
            view=0;
            LCD_Clear(Black);
        }
    }
    else if(1==key[3].single_flag)
    {
        key[3].single_flag=0;
    }
    else
    {
        
        if(uwTick_sleep>=500)//5S无按键操作
        {
            if(0==view)
           LCD_Clear(Black); 
            view=1;            
        }
    }
    
}

void usart_proc(void)
{
    if(rx_pointer!=0)
    {
        if(strcmp((char *)rxdata,"B1")==0)//实现按键B1切换模式功能
        {
            usart_flag=1;
            usart_led=1;
        }
        else if(strcmp((char *)rxdata,"B2")==0)//实现B2档位加功能
        {
            usart_flag=2;
            usart_led=1;
        }
        else if(strcmp((char *)rxdata,"B3")==0)//实现B3档位减功能
        {
            usart_flag=3;
            usart_led=1;
        }
        else
        {
            memset(usart_text,0,sizeof(usart_text));
            sprintf((char *)usart_text,"NULL\r\n");
            HAL_UART_Transmit(&huart1,usart_text,6,50);
        }
        rx_pointer=0;
        memset(rxdata,0,sizeof(rxdata));
    }
}

void pwm_proc(void)
{
    //占空比=比较值/重装载值
    switch(gear)
    {
        case 1:
            //调整占空比:10%
            __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,10.0/100*(TIM2->ARR+1));
            break;
        case 2:
            //调整占空比:40%
            __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,40.0/100*(TIM2->ARR+1));
            break;
        case 3:
            //调整占空比:80%
            __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,80.0/100*(TIM2->ARR+1));
            break;
    }
//            memset(lcd_text,0,sizeof(lcd_text));
//            sprintf((char *)lcd_text,"     PWM:%0.1f          ",(TIM2->CCR1+1)*100.0/(TIM2->ARR+1));
//            LCD_DisplayStringLine(Line7,lcd_text);
}

void led_proc(void)
{
    if((uwTick-uwTick_Led)<50)return;
    uwTick_Led=uwTick;
    
    if(0==mode)
    {
        ucled|=0x80;
    }
    else
    {
        ucled&=0x7f;
    }
    
    switch(gear)
    {
        case 1://1档
            ucled|=0x01;
            ucled&=0xf9;
            break;
        case 2://2档
            ucled|=0x02;
            ucled&=0xfa;
            break;
        case 3://3档
            ucled|=0x04;
            ucled&=0xfc;
            break;
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
