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
#include "lcd.h"
#include "led.h"
#include "timer.h"
#include "myusart.h"

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
void usart_proc(void);
void led_proc(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

//led灯
unsigned char ucled=0x00;

//lcd显示屏的数据
unsigned char lcd_text[30];

//减速函数
__IO uint32_t uwTick_lcd;
__IO uint32_t uwTick_led;

//界面切换参数
unsigned char view=0;

//密码
char mima[4]={'1','2','3'};

//按键输入的密码
//如密码正确，LCD 显示跳转到输出状态界面
//否则停留在密码输入界面（如图 2 所示），显示界面的三位密码值重置为@
char mima_in[4]={'@','@','@'};

unsigned int pa1_frq=1000;
unsigned int pa1_duty=500;

//设备上电或重新进入密码输入界面，显示界面的密码值重置为字符@。
unsigned char reset_flag=0;

//计算连续输入错误密码的个数
unsigned char count=0;

//连续输入三个以上错误密码的标志
unsigned char flag=0;

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
    
    //使能定时器4
    HAL_TIM_Base_Start_IT(&htim4);
    
    //PWM波
    HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
    
    //初始化串口接收
    HAL_UART_Receive_IT(&huart1,&rxdat,1);
    
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
      key_proc();
      led_proc();
      //防止接收不完整
      if(rx_pointer!=0)
      {
          int temp=rx_pointer;
          HAL_Delay(1);
          if(temp==rx_pointer)
          {
              usart_proc();
          }
      }
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
void led_proc(void)
{
    if((uwTick-uwTick_led)<50)return;
    uwTick_led=uwTick;
    
    led_disp(ucled);
}


void usart_proc(void)
{
    if(rx_pointer!=0)
    {
        if((rx_pointer==7)&&(rxdata[3]=='-'))
        {
            char  oldmima[4];
            //输入的新密码
            char  newmima[4];
            char data[255];
            sscanf(rxdata,"%3s-%3s",oldmima,newmima);
            
            memset(data,NULL,sizeof(data));
            sprintf(data,"mima:%s\r\n",mima);
            HAL_UART_Transmit(&huart1,(uint8_t *)data,strlen(data),50);
            
           memset(data,NULL,sizeof(data));
           sprintf(data,"%s\r\n",rxdata);
            HAL_UART_Transmit(&huart1,(uint8_t *)data,strlen(data),50);


            if(0==strcmp(oldmima,mima))
            {
                
                strcpy(mima,newmima);
                memset(data,NULL,sizeof(data));

                sprintf(data,"\r\n new password:%s\r\n\n",mima);
                HAL_UART_Transmit(&huart1,(uint8_t *)data,strlen(data),50);
            }
            else
            {
                memset(data,NULL,sizeof(data));
                sprintf(data,"password error!\r\n");
                HAL_UART_Transmit(&huart1,(uint8_t *)data,strlen(data),50);
            }
        }
        else
        {
            char data[30]="passin error!\r\n";
            HAL_UART_Transmit(&huart1,(uint8_t *)data,strlen(data),50);
        }
        rx_pointer=0;
        memset(rxdata,0,sizeof(rxdata));
    }
}

void key_proc(void)
{
    if(1==key[0].single_flag)
    {
        key[0].single_flag=0;
        if(0==view){//密码输入界面
            if('@'==mima_in[0])mima_in[0]='0';
            else
            {
                if(mima_in[0]=='9')mima_in[0]='0';
                else
                 mima_in[0]+=1;
                
            }
      }
    }
    else if(1==key[1].single_flag)
    {  
        key[1].single_flag=0;
        if(0==view){//密码输入界面
            if('@'==mima_in[1])mima_in[1]='0';
            else
            {
                if(mima_in[1]=='9')mima_in[1]='0';
                else
                 mima_in[1]+=1;
                
            }
     }
    }
    else if(1==key[2].single_flag)
    {
        key[2].single_flag=0;
        if(0==view){//密码输入界面
            if('@'==mima_in[2])mima_in[2]='0';
            else
            {
                if(mima_in[2]=='9')mima_in[2]='0';
                else
                 mima_in[2]+=1; 
            }
        }
    }
    else if(1==key[3].single_flag)
    {
        key[3].single_flag=0;
        if(0==view){
        //如密码正确，LCD 显示跳转到输出状态界面
        if(0==strcmp(mima_in,mima))
        {
            LCD_Clear(Black);
            count=0;
           // flag=0;
            view=1;
        }
        else//否则停留在密码输入界面（如图 2 所示），显示界面的三位密码值重置为@。
        {
           count++;
            if(count>=3)flag=1;
           memset(mima_in,'@',sizeof(mima_in)-1);
        }
      }
    }
    
}

void lcd_proc(void)
{
    if((uwTick-uwTick_lcd)<100)return;
    uwTick_lcd=uwTick;
   
    switch(view)
    {
        case 0://密码输入界面
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"       PSD           ");
            LCD_DisplayStringLine(Line1,lcd_text);
        
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"    B1:%c       ",mima_in[0]);
            LCD_DisplayStringLine(Line3,lcd_text);

            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"    B2:%c       ",mima_in[1]);
            LCD_DisplayStringLine(Line4,lcd_text);

            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"    B3:%c       ",mima_in[2]);
            LCD_DisplayStringLine(Line5,lcd_text);  
            
        
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"    mima:%s              ",mima);
            LCD_DisplayStringLine(Line9,lcd_text);
        
            break;
        
        case 1://输出状态界面
            //频率=80MHZ/预分频系数/重装载值
            pa1_frq=(80000000/(TIM2->PSC+1)/(TIM2->ARR+1));
            //占空比=输出比较值/重装载值
            //TIM2->CCR2：定时器2的捕获/比较寄存器的值
            pa1_duty=(100*TIM2->CCR2)/(TIM2->ARR+1);
            
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"       STA           ");
            LCD_DisplayStringLine(Line1,lcd_text);
            
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"    F:%d              ",pa1_frq);
            LCD_DisplayStringLine(Line3,lcd_text);
        
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"    D:%d%%              ",pa1_duty);
            LCD_DisplayStringLine(Line4,lcd_text);
        
            break;
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
