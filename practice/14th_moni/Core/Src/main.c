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
#include "led.h"
#include "lcd.h"
#include "myadc.h"
#include "myusart.h"
#include "timer.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

__IO uint32_t uwTick_Lcd;
__IO uint32_t uwTick_Led;

unsigned char ucled=0x00;
unsigned char lcd_text[30];
unsigned char usart_text[10];

double VR37;//电位器R37输出的模拟电压信号VR37
double temp_v=0.3;
unsigned int frq_pa7=1000;

unsigned char view=0;
unsigned char flag=0;
unsigned int duty_pa7=10;//pa7的占空比

unsigned char led3_flag=0;

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
  MX_ADC2_Init();
  MX_TIM4_Init();
  MX_TIM17_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
    led_disp(ucled);
    
    LCD_Init();
    LCD_Clear(Black);
    LCD_SetBackColor(Black);
    LCD_SetTextColor(White);
    
    HAL_TIM_Base_Start_IT(&htim4);
    
    HAL_TIM_PWM_Start(&htim17,TIM_CHANNEL_1);
   
   HAL_UART_Receive_IT(&huart1,&rxdat,1);    
    
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
      key_proc();
      lcd_proc();
      led_proc();
      if(rx_pointer!=0)
      {
          int temp=rx_pointer;
          HAL_Delay(5);////延时两个毫秒，如果没有接收完成，中断回调函数会改变rxPoint的值，导致len！=rxPoint
          if(temp==rx_pointer)
          {
             usart_proc(); 
          }
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

void lcd_proc(void)
{
    if((uwTick-uwTick_Lcd)<100)return;
    uwTick_Lcd=uwTick;
    
    VR37=get_Adc(&hadc2);
    
    if(VR37>temp_v)led3_flag=1;
    
    switch(view)
    {
        case 0:
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"        DATA          ");
            LCD_DisplayStringLine(Line1,lcd_text);
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"      VR37:%0.2f         ",VR37);
            LCD_DisplayStringLine(Line3,lcd_text);
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"      PA7:%dHZ          ",frq_pa7);
            LCD_DisplayStringLine(Line4,lcd_text);
        
            break;
        case 1:
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"        PARA          ");
            LCD_DisplayStringLine(Line1,lcd_text);
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"      VP1:%0.2f         ",temp_v);
            LCD_DisplayStringLine(Line3,lcd_text);
            break;
    }

}

void key_proc(void)
{
    //PSC:预分频系数，arr:重装载值
   //PA7的频率
    //frq_pa=80000000/((TIM17->PSC+1)*(TIM17->ARR+1));

    if(1==key[0].single_flag)//切换按键
    {
        key[0].single_flag=0;
        view++;
        if(view==2)view=0;
        LCD_Clear(Black);
    }
    else if(1==key[1].single_flag)//电压参数调整
    {
        key[1].single_flag=0;
        if(1==view)
          {
              temp_v+=0.3;
              if(temp_v>3.3)temp_v=0;
          }
        
    }
    else if(1==key[2].single_flag)//频率调整
    {
        key[2].single_flag=0;
        if(0==view)
        {
            
            frq_pa7+=1000;
            if(frq_pa7>=11000)frq_pa7=1000;
            
            //调整重载载值
            //htim是一个指向定时器句柄的指针，Autoreload是重装载值。
            //频率=80MZ/（预分频系数+1）/（重装载值+1）
            __HAL_TIM_SetAutoreload(&htim17,(80000000.0/frq_pa7/(TIM17->PSC+1))-1);
            //刷新定时器的计数器和比较器的值
            HAL_TIM_GenerateEvent(&htim17,TIM_EventSource_Update);
            //调整占空比
            //占空比=比较值/重装载值
            __HAL_TIM_SetCompare(&htim17,TIM_CHANNEL_1,(duty_pa7/100.0*TIM17->ARR));
            
        }
    }
    else if(1==key[3].single_flag)
    {
        key[3].single_flag=0;
    }
}

void usart_proc(void)
{
    if(rx_pointer!=0)
    {
        
        if(rxdata[0]>='1'&&rxdata[0]<='9'&&rx_pointer==1)
           {
               duty_pa7=(rxdata[0]-'0')*10;
              //修改占空比
               //占空比=比较值/重装载值
               __HAL_TIM_SetCompare(&htim17,TIM_CHANNEL_1,(duty_pa7/100.0)*(TIM17->ARR+1));
          }
        else
        {
                  
           memset(usart_text,0,sizeof(usart_text));
           sprintf((char *)usart_text,"error\r\n");
           HAL_UART_Transmit(&huart1,usart_text,sizeof(usart_text),50);
        }       

        rx_pointer=0;
        memset(rxdata,0,sizeof(rxdata));   
    }
}

void led_proc(void)
{
    if((uwTick-uwTick_Led)<50)return;
    uwTick_Led=uwTick;
    
    if(0==view)
    {
        ucled|=0x01;
    }
    else
    {
        ucled&=0xfe;
    }
    if(1==view)
    {
        ucled|=0x02;
    }
    else
    {
        ucled&=0xfd;
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
