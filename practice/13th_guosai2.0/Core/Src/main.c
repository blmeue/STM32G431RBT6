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
#include "i2c_hal.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

__IO uint32_t uwTick_Lcd;
__IO uint32_t uwTick_Led;
__IO uint32_t uwTick_Pa7;

unsigned char view;
unsigned char view_mode;

unsigned char ucled=0x00;
unsigned char lcd_text[30];
unsigned char usart_text[30];

unsigned int X=1;
unsigned int Y=1;

double PA4_data;
double PA5_data;

unsigned int N_pa4;
double A_pa4;
double T_pa4;
double H_pa4;

unsigned int N_pa5;
double A_pa5;
double T_pa5;
double H_pa5;

unsigned char mode=0;
unsigned char channel=0;
double sum_pa4;
double sum_pa5;
double value[2];

unsigned char LD3_flag=0;

//unsigned int pa7_frq;
unsigned int pre;
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

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void led_proc(void);
void lcd_proc(void);
void key_proc(void);
void usart_proc(void);
void PA7_proc(void);

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
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
    
    led_disp(ucled);
    
    LCD_Init();
    LCD_Clear(Black);
    LCD_SetBackColor(Black);
    LCD_SetTextColor(White);
    
    HAL_TIM_Base_Start_IT(&htim4);
    
    HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_2);
    
    HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2);
    
    HAL_UART_Receive_IT(&huart1,&rxdat,1);

    //第一次上电
    if(eeprom_read(0x02)!=22&&eeprom_read(0x03)!=33)
    {
        eeprom_write(0x02,22);
        HAL_Delay(2);
        eeprom_write(0x03,33);
        HAL_Delay(2);
        eeprom_write(1,X);//X频率参数
        HAL_Delay(2);
        eeprom_write(0,Y);//Y电压参数
        HAL_Delay(2);
    }
    else//不是第一次上电
    {
        X=eeprom_read(1);
        HAL_Delay(2);
        Y=eeprom_read(0);
        HAL_Delay(2);
    }
    PA7_proc();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
      
      key_proc();
      //PA7_proc();
      lcd_proc();
      led_proc();
      
      if(rx_pointer!=0)
      {
          int temp=rx_pointer;
          HAL_Delay(2);
          if(temp==rx_pointer)usart_proc();
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
void led_proc(void)
{
    if((uwTick-uwTick_Led)<50)return;
    uwTick_Led=uwTick;
    
    
    //倍频模式
    if(0==mode)
    {
        ucled|=0x01;
    }
    else
    {
        ucled&=0xfe;
    }
    
    //分频模式
    if(1==mode)
    {
        ucled|=0x02;
    }
    else
    {
        ucled&=0xfd;
    }
    
    if(value[0]>(1.0*value[1]*Y))
    {
        LD3_flag=1;
    }
    else
    {
        LD3_flag=0;
    }
    
    //正向显示模式
    if(0==view_mode)
    {
        ucled|=0x08;
    }
    else
    {
        ucled&=0xf7;
    }
    
    led_disp(ucled);
}

void PA7_proc(void)
{
    //if((uwTick-uwTick_Pa7)<50)return;
    //uwTick_Pa7=uwTick;
    
    //频率=（80000000/预分频系数）/重装载值
   
    //倍频模式   
    if(0==mode)
    {
        pre=(80000000/100)/(pa1_frq*X)-1;
        __HAL_TIM_SET_PRESCALER(&htim3,pre);
         HAL_TIM_GenerateEvent(&htim3, TIM_EVENTSOURCE_UPDATE);
    }
    //分频模式下
    else
    {
        pre=(80000000/100)/(pa1_frq/X)-1;
        __HAL_TIM_SET_PRESCALER(&htim3,pre);
        HAL_TIM_GenerateEvent(&htim3, TIM_EVENTSOURCE_UPDATE);
    }
}

void lcd_proc(void)
{
    if((uwTick-uwTick_Lcd)<100)return;
    uwTick_Lcd=uwTick;
    
    if(0==view_mode)//正向显示
    {
        LCD_WriteReg(R1,0X0000);
        LCD_WriteReg(R96,0X2700);
    }
    else//翻转显示
    {
        LCD_WriteReg(R1,0X0100);
        LCD_WriteReg(R96,0XA700);
    }
    
    switch(view)
    {
        case 0://数据界面
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"        DATA       ");
            LCD_DisplayStringLine(Line1,lcd_text);
            
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     PA4=%0.2f        ",value[0]);
            LCD_DisplayStringLine(Line3,lcd_text);
        
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     PA5=%0.2f        ",value[1]);
            LCD_DisplayStringLine(Line4,lcd_text);
        
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     PA1=%d        ",pa1_frq);
            LCD_DisplayStringLine(Line5,lcd_text);
        
            break;
        case 1://参数界面
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"        PARA       ");
            LCD_DisplayStringLine(Line1,lcd_text);
           
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     X=%d          ",X);
            LCD_DisplayStringLine(Line3,lcd_text);
           
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     Y=%d          ",Y);
            LCD_DisplayStringLine(Line4,lcd_text);
           
            break;
        case 2://记录界面
            //PA4通道数据
            if(0==channel)
            {
                memset(lcd_text,0,sizeof(lcd_text));
                sprintf((char *)lcd_text,"        REC-PA4        ");
                LCD_DisplayStringLine(Line1,lcd_text);
                
                memset(lcd_text,0,sizeof(lcd_text));
                sprintf((char *)lcd_text,"     N=%d          ",N_pa4);
                LCD_DisplayStringLine(Line3,lcd_text);
                
                memset(lcd_text,0,sizeof(lcd_text));
                sprintf((char *)lcd_text,"     A=%0.2f        ",A_pa4);
                LCD_DisplayStringLine(Line4,lcd_text);
            
                memset(lcd_text,0,sizeof(lcd_text));
                sprintf((char *)lcd_text,"     T=%0.2f        ",T_pa4);
                LCD_DisplayStringLine(Line5,lcd_text);
            
                memset(lcd_text,0,sizeof(lcd_text));
                sprintf((char *)lcd_text,"     H=%0.2f        ",H_pa4);
                LCD_DisplayStringLine(Line6,lcd_text);
                
              
            }
            //PA5通道数据
            else if(1==channel)
            {
                memset(lcd_text,0,sizeof(lcd_text));
                sprintf((char *)lcd_text,"        REC-PA5        ");
                LCD_DisplayStringLine(Line1,lcd_text);
                
                memset(lcd_text,0,sizeof(lcd_text));
                sprintf((char *)lcd_text,"     N=%d          ",N_pa5);
                LCD_DisplayStringLine(Line3,lcd_text);
                
                memset(lcd_text,0,sizeof(lcd_text));
                sprintf((char *)lcd_text,"     A=%0.2f        ",A_pa5);
                LCD_DisplayStringLine(Line4,lcd_text);
            
                memset(lcd_text,0,sizeof(lcd_text));
                sprintf((char *)lcd_text,"     T=%0.2f        ",T_pa5);
                LCD_DisplayStringLine(Line5,lcd_text);
            
                memset(lcd_text,0,sizeof(lcd_text));
                sprintf((char *)lcd_text,"     H=%0.2f        ",H_pa5);
                LCD_DisplayStringLine(Line6,lcd_text);
            }
            break;
    }
              PA7_proc();  
}

void key_proc(void)
{
    if(1==key[0].single_flag)
    {
        key[0].single_flag=0;
        if(1==view)channel=0;
        
        view++;
        
        if(3==view)view=0;
        LCD_Clear(Black);
    }
    else if(1==key[1].single_flag)
    {
        key[1].single_flag=0;
        if(1==view)
        {
            X++;
            if(5==X)X=1;
            eeprom_write(1,X);
            HAL_Delay(2);
            PA7_proc();
        }
    }
    else if(1==key[2].single_flag)
    {
        key[2].single_flag=0;
        if(1==view)
        {
            Y++;
            if(5==Y)Y=1;
            eeprom_write(0,Y);
            HAL_Delay(2);
        }
    }
    else if(1==key[3].single_flag)
    {
        key[3].single_flag=0;
        switch(view)
        {
            case 0:
                Get_adc(&hadc2,value);
            
                N_pa4++;
                N_pa5++;
                sum_pa4+=value[0];
                sum_pa5+=value[1];
                    
                if(value[0]>A_pa4)
                {
                    A_pa4=value[0];
                }
                if(0==T_pa4)
                {
                    T_pa4=value[0];
                }
                else if(T_pa4>value[0])
                {
                    T_pa4=value[0];
                }
            
                if(value[1]>A_pa5)
                {
                    A_pa5=value[1];
                }
                
                if(0==T_pa5)
                {
                    T_pa5=value[1];
                }
                else if(T_pa5>value[1])
                {
                    T_pa5=value[1];
                }
                
                H_pa4=1.0*sum_pa4/N_pa4;
                H_pa5=1.0*sum_pa5/N_pa5;
                
                break;
            case 1:
                mode=!mode;
                PA7_proc();
                break;
            case 2:
                channel=!channel;
                LCD_Clear(Black);
              
               break;
        }
        
    }
     if(1==key[3].long_flag)
        {
            key[3].long_flag=0;
            if(2==view)
            {
                
                    N_pa4=0;
                    A_pa4=0;
                    T_pa4=0;
                    sum_pa4=0;
                    H_pa4=0; 
               
                    N_pa5=0;
                    A_pa5=0;
                    T_pa5=0;
                    sum_pa5=0;
                    H_pa5=0;
                
            }
        }
}

void usart_proc(void)
{
    if(rx_pointer!=0)
    {
        if(0==strcmp((char *)rxdata,"#"))
        {
            view_mode=!view_mode;
            LCD_Clear(Black);
        }
        else if(0==strcmp((char *)rxdata,"X"))
        {
            memset(usart_text,0,sizeof(usart_text));
            sprintf((char *)usart_text,"X:%d",X);
            HAL_UART_Transmit(&huart1,usart_text,3,50);
        }
        else if(0==strcmp((char *)rxdata,"Y"))
        {
            memset(usart_text,0,sizeof(usart_text));
            sprintf((char *)usart_text,"Y:%d",Y);
            HAL_UART_Transmit(&huart1,usart_text,3,50);
        }
        else if(0==strcmp((char *)rxdata,"PA1"))
        {
            memset(usart_text,0,sizeof(usart_text));
            sprintf((char *)usart_text,"PA1:%d",pa1_frq);
            HAL_UART_Transmit(&huart1,usart_text,10,50);
        }
        else if(0==strcmp((char *)rxdata,"PA4"))
        {
            memset(usart_text,0,sizeof(usart_text));
            sprintf((char *)usart_text,"PA4:%0.2f",value[0]);
            HAL_UART_Transmit(&huart1,usart_text,8,50);
        }
        else if(0==strcmp((char *)rxdata,"PA5"))
        {
            memset(usart_text,0,sizeof(usart_text));
            sprintf((char *)usart_text,"PA5:%0.2f",value[1]);
            HAL_UART_Transmit(&huart1,usart_text,8,50);
        }
        
        rx_pointer=0;
        memset(rxdata,0,sizeof(rxdata));
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
