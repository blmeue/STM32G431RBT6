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

#include "stdio.h"
#include "String.h"
#include "led.h"
#include "myusart.h"
#include "timer.h"
#include "lcd.h"
#include "i2c - hal.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

unsigned char ucled=0x00;

unsigned char lcd_text[30];

unsigned char view=0;

__IO uint32_t uwTick_Lcd;
__IO uint32_t uwTick_Led;
__IO uint32_t uwTick_eeprom;

//商品X\Y的数量
unsigned int X_number=0;
unsigned int Y_number=0;

//商品X\Y的库存数量
unsigned int X_reserve=10;
unsigned int Y_reserve=10;

//串口输出信息
unsigned char usart_text[20];

//商品X\Y的价格
double X_price=1.0;
double Y_price=1.0;

//B4按键按下标志
unsigned char B4_flag=0;

//商品X、Y库存数量均为0标志
unsigned char Reserve_0_flag=0;

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
void eeprom_proc(void);
void usart_proc(void);

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
  MX_USART1_UART_Init();
  MX_TIM3_Init();
  MX_TIM17_Init();
  /* USER CODE BEGIN 2 */
    I2CInit();
    
    led_disp(ucled);
    
    LCD_Init();
    LCD_Clear(Black);
    LCD_SetBackColor(Black);
    LCD_SetTextColor(White);
    
    //使能
    HAL_TIM_Base_Start_IT(&htim3);
    
    //开启串口接收
    HAL_UART_Receive_IT(&huart1,&rxdat,1);
    
    //使能PWM
    HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
    
    //判断是否为第一次上电
    //如果数据不是12，说明设备是第一次上电，先将初始值写入地址中，
    //再将标志位12写入地址中，表明此后将不是第一次上电。
    unsigned char value=eeprom_read(4);
    if(value!=12)//第一次上电
    {
       eeprom_proc();
       eeprom_write(4,12);
        HAL_Delay(10);
    }
    else
    {
        X_reserve=eeprom_read(0);
        HAL_Delay(10);
        Y_reserve=eeprom_read(1);
        HAL_Delay(10);
        X_price=eeprom_read(2)/10.0;
        HAL_Delay(10);
        Y_price=eeprom_read(3)/10.0;
        HAL_Delay(10);
    }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
      key_proc();
      lcd_proc();
      if(rx_pointer>0)
      {
          int temp=rx_pointer;
          HAL_Delay(1);
          if(temp==rx_pointer)
              usart_proc();
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
    
    switch(view)
    {
        case 0://商品购买界面
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"        SHOP           ");
            LCD_DisplayStringLine(Line1,lcd_text);
   
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     X:%d        ",X_number);
            LCD_DisplayStringLine(Line3,lcd_text);
 
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     Y:%d        ",Y_number);
            LCD_DisplayStringLine(Line4,lcd_text);
         
            break;
        case 1://商品价格界面
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"        PRICE           ");
            LCD_DisplayStringLine(Line1,lcd_text);
   
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     X:%0.1f        ",X_price);
            LCD_DisplayStringLine(Line3,lcd_text);
 
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     Y:%0.1f        ",Y_price);
            LCD_DisplayStringLine(Line4,lcd_text);

            break;
        case 2://库存信息界面
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"        REP            ");
            LCD_DisplayStringLine(Line1,lcd_text);
   
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     X:%d        ",X_reserve);
            LCD_DisplayStringLine(Line3,lcd_text);
 
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     Y:%d        ",Y_reserve);
            LCD_DisplayStringLine(Line4,lcd_text);
             
            break;
    }
}

void key_proc(void)
{
    if(1==key[0].single_flag)//B1:界面切换按键
    {
        
        key[0].single_flag=0;
        
        view++;
        if(3==view)view=0;
        
        LCD_Clear(Black);
        
    }
    else if(1==key[1].single_flag)
    {
        key[1].single_flag=0;
        
        switch(view)
        {
            case 0://商品购买界面下
                X_number++;
                if(X_number>X_reserve)
                    X_number=0;
                break;
            case 1://商品价格界面下
                X_price+=0.1;
                if(X_price>=2.1)
                    X_price=1.0;
                eeprom_proc();
                break;
            case 2://库存信息界面下
                X_reserve++;
                eeprom_proc();
                break;
        }
    }
    else if(1==key[2].single_flag)
    {
        key[2].single_flag=0;
        switch(view)
        {
            case 0://商品购买界面下
                Y_number++;
                if(Y_number>Y_reserve)
                    Y_number=0;
                break;
            case 1://商品价格界面下
                Y_price+=0.1;
                if(Y_price>=2.1)
                    Y_price=1.0;
                eeprom_proc();
                break;
            case 2://库存信息界面下
                Y_reserve++;
                eeprom_proc();
                break;
        }
    }
    else if(1==key[3].single_flag)
    {
        key[3].single_flag=0;
        B4_flag=1;
        if(0==view)//商品购买界面下，按下B4，确认购买信息
        {
            X_reserve=X_reserve-X_number;
            Y_reserve=Y_reserve-Y_number;
            //B4按键按下后，设备串口输出购买商品数量和总价格
            
            double sum=X_number*X_price+Y_number*Y_price;
            memset(usart_text,0,sizeof(usart_text));
            sprintf((char *)usart_text,"X:%d,Y:%d,Z:%0.1f",X_number,Y_number,sum); 
            
            HAL_UART_Transmit(&huart1,usart_text,sizeof(usart_text),50);
            
            X_number=0;
            Y_number=0;
            eeprom_proc();

            
        }
    }
    if((0==X_reserve)&&(0==Y_reserve))
    {
        Reserve_0_flag=1;
    }
    else
    {
        Reserve_0_flag=0;
    }
}

void eeprom_proc(void)
{
   if((uwTick-uwTick_eeprom)<50)return;
    uwTick_eeprom=uwTick;   
        
    eeprom_write(0,X_reserve);//地址0，存储商品X库存数量
    HAL_Delay(10);
    
    eeprom_write(1,Y_reserve); //地址1，存储商品Y库存数量  
    HAL_Delay(10);
    
    
    eeprom_write(2,X_price*10);//地址2：存储商品X单价
    HAL_Delay(10);
    
    eeprom_write(3,Y_price*10); //地址3：存储商品Y单价  
    HAL_Delay(10);
    
}

void usart_proc(void)
{
    if(rx_pointer>0)
    {
        if(strcmp((char *)rxdata,"?")==0)
        {
            memset(usart_text,0,sizeof(usart_text));
            sprintf((char *)usart_text,"X:%0.1f,Y:%0.1f",X_price,Y_price);
            HAL_UART_Transmit(&huart1,usart_text,sizeof(usart_text),50);
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
