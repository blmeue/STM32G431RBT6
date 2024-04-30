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
#include "String.h"
#include "led.h"
#include "lcd.h"
#include "myadc.h"
#include "myusart.h"
#include "timer.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

unsigned char ucled=0x00;
unsigned char lcd_text[30];
unsigned char usart_text[30];

//减速函数
__IO uint32_t uwTick_Led;
__IO uint32_t uwTick_Lcd;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

unsigned char view=0;

double R38_v=0.0;
double R37_v=0.0;

//R37产品检测标准上限
double R37_H=2.2;
//R37产品检测标准下限
double R37_L=1.2;

//R38产品检测标准上限
double R38_H=3.0;
//R37产品检测标准下限
double R38_L=1.4;

//R37产品合格率
double PR37=0;
//R38产品合格率
double PR38=0;

//R37的总检测数
unsigned int R37_Count=0;
//R37的符合标准数
unsigned int R37_Standard=0;

//R38的总检测数
unsigned int R38_Count=0;

//R38的符合标准数
unsigned int R38_Standard=0;

//产品R37检测通过标志
unsigned char R37_Pass=0;

//产品R38检测通过标志
unsigned char R38_Pass=0;


unsigned char choose_key=0;

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
void key_proc(void);
void Led_proc(void);
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
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_USART1_UART_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
    
    led_disp(ucled);
    
    LCD_Init();
    LCD_Clear(Black);
    LCD_SetBackColor(Black);
    LCD_SetTextColor(White);
    
    //使能时钟TIM3
    HAL_TIM_Base_Start_IT(&htim3);
    
    //打开串口接收
    HAL_UART_Receive_IT(&huart1,&rxdat,1);
    //默认产品参数界面
    view=0;
    
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
      key_proc();
      Lcd_proc();
      Led_proc();
      if(rx_pointer!=0)
      {
          int temp=rx_pointer;
          HAL_Delay(1);
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

void Lcd_proc(void)
{
    if((uwTick-uwTick_Lcd)<100)return;
    uwTick_Lcd=uwTick;
    

    
    switch(view)
    {
        case 0://产品参数界面
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"       GOODS          ");
            LCD_DisplayStringLine(Line1,lcd_text);
            
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     R37:%0.2fV",R37_v);
            LCD_DisplayStringLine(Line3,lcd_text);
        
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     R38:%0.2fV",R38_v);
            LCD_DisplayStringLine(Line4,lcd_text);
                
            break;
        case 1://标准设置界面
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"      STANDARD       ");
            LCD_DisplayStringLine(Line1,lcd_text);
            
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"    SR37:%0.1f-%0.1f",R37_L,R37_H);
            LCD_DisplayStringLine(Line3,lcd_text);
        
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"    SR38:%0.1f-%0.1f",R38_L,R38_H);
            LCD_DisplayStringLine(Line4,lcd_text);
         
            break;
        case 2://合格率界面
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"        PASS          ");
            LCD_DisplayStringLine(Line1,lcd_text);
                        
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     PR37:%0.1f%%       ",PR37);
            LCD_DisplayStringLine(Line3,lcd_text);
        
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     PR38:%0.1f%%       ",PR38);
            LCD_DisplayStringLine(Line4,lcd_text);
         
            break;
    }
}

void key_proc(void)
{
    R37_v=Get_Adc(&hadc2);
    R38_v=Get_Adc(&hadc1);
    HAL_Delay(1);
    
    if(1==keys[0].single_flag)//按下了B1
    {
        keys[0].single_flag=0;
        view++;
        if(3==view)view=0;
        //每转换一次界面，就清一次屏
         LCD_Clear(Black);
    }
    else if(1==keys[1].single_flag)//按下了B2
    {
        keys[1].single_flag=0;
        if(0==view)//产品界面下，检测产品R37是否合格
        {
            R37_Count++;
            if((R37_v>=R37_L)&&(R37_v<=R37_H))
            {
                R37_Standard++;
                R37_Pass=1;
            }
            //if(R37_Count!=0)
            PR37=((1.0*R37_Standard)/R37_Count)*100;
        }
        else if(1==view)//标准设置界面下，选择需要调整产品R37\R38的检测标准上限
        {
           choose_key++;
           if(4==choose_key)choose_key=0;
        }
    }
    else if(1==keys[2].single_flag)//按下了B3
    {
        keys[2].single_flag=0;
        if(0==view)//产品界面下，检测产品R38是否合格
        {
            R38_Count++;
            if((R38_v>=R38_L)&&(R38_v<=R38_H))
            { 
                R38_Standard++;
                R38_Pass=1;
            }
            //if(R38_Count!=0)
            PR38=((1.0*R38_Standard)/R38_Count)*100;
         }
        else if(1==view)//标准设置界面下，选择的产品标准上限或下限加0.2，取值范围：上限：2.2-3.0；下限：1.2-2.0
        {
            switch(choose_key)
            {
                case 0://产品R37上限
                    R37_H+=0.2;
                    if(R37_H>=3.2)
                        R37_H=2.2;
                    PR37=0;R37_Standard=0;R37_Count=0;
                    break;
                case 1://产品R37下限
                    R37_L+=0.2;
                    if(R37_L>2.0)
                        R37_L=1.2;
                    PR37=0;R37_Standard=0;R37_Count=0;
                    break;
                case 2://产品R38上限
                    R38_H+=0.2;
                    if(R38_H>=3.2)
                        R38_H=2.2;
                    PR38=0;R38_Standard=0;R38_Count=0;
                    break;
                case 3://产品R38下限
                    R38_L+=0.2;
                    if(R38_L>2.0)
                        R38_L=1.2;
                    PR38=0;R38_Standard=0;R38_Count=0;
                    break;
            }
        }
    }
    else if(1==keys[3].single_flag)//按下了B4
    {
        keys[3].single_flag=0;
        if(1==view)//标准设置界面下，选择的产品标准上限或下限减0.2，上限取值范围：3.0-2.2；下限取值范围：2.0-1.2
        {
            switch(choose_key)
            {
                case 0://产品R37上限
                    R37_H-=0.2;
                    if(R37_H<=2.0)
                        R37_H=3.0;
                    PR37=0;R37_Standard=0;R37_Count=0;
                    break;
                case 1://产品R37下限
                    R37_L-=0.2;
                    if(R37_L<1.2)
                        R37_L=2.0;
                    PR37=0;R37_Standard=0;R37_Count=0;
                    break;
                case 2://产品R38上限
                    R38_H-=0.2;
                    if(R38_H<=2.0)
                        R38_H=3.0;
                    PR38=0;R38_Standard=0;R38_Count=0;
                    break;
                case 3://产品R38下限
                    R38_L-=0.2;
                    if(R38_L<1.2)
                        R38_L=2.0;
                    PR38=0;R38_Standard=0;R38_Count=0;
                    break;
            }
        }
        else if(2==view)//合格率界面下，清零当前得出的产品R37和R38的合格率
        {
            PR37=0;
            PR38=0;
            R38_Standard=0;
            R38_Count=0;
            R37_Standard=0;
            R37_Count=0;
        }
    }

}

void Led_proc(void)
{
    if((uwTick-uwTick_Led)<50)return;
    uwTick_Led=uwTick;
    
//    if(1==R37_Pass)//产品R37检测通过时，LD1点亮1s后熄灭；
//    {
//        ucled |=0x01;
//    }
//    else 
//    {
//        ucled &=0xfe;
//    }
//    
//    if(1==R38_Pass)//产品R38检测通过时，LD2点亮1s后熄灭；
//    {
//        ucled |=0x02;
//    }
//    else
//    {
//        ucled &=0xfd;
//    }
    
    switch(view)
    {
        case 0://产品参数界面下，LD3点亮，否则熄灭
            ucled|=0x04;
            ucled &=0xe7;
            break;
        case 1://标准设置界面下，LD4点亮，否则熄灭
            ucled |=0x08;
            ucled &=0xeb;
            break;
        case 2://合格率界面下，LD5点亮，否则熄灭
            ucled |=0x10;
            ucled &=0xf3;
            break;
    }
    led_disp(ucled);
}

void usart_proc(void)
{
    if(rx_pointer!=0)
    {
//串口接收到字符“R37”或“R38”后，向PC端返回对应产品的名称、当前总检测数、合格数及合格率。
//返回格式如下：
//产品名称:总检测数,合格数,合格率
        if(strcmp((char *)rxdata,"R37")==0)
        {
        memset(usart_text,0,sizeof(usart_text));    
        sprintf((char *)usart_text,"R37:%d,%d,%0.1f%%",R37_Count,R37_Standard,PR37);
        HAL_UART_Transmit(&huart1,usart_text,sizeof(usart_text),50);
        }
        else if(strcmp((char *)rxdata,"R38")==0)
        {
        memset(usart_text,0,sizeof(usart_text));    
        sprintf((char *)usart_text,"R38:%d,%d,%0.1f%%",R38_Count,R38_Standard,PR38);
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
