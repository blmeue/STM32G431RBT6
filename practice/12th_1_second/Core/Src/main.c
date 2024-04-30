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
#include "string.h"

#include "led.h"
#include "lcd.h"
#include "myusart.h"
#include "timer.h"


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

unsigned char ucled=0x00;
unsigned char lcd_text[30];
unsigned char usart_text[30];

__IO uint32_t uwTick_Lcd;
__IO uint32_t uwTick_Led;

unsigned char view=0;

unsigned int CNBR_Data;
unsigned int VNBR_Data;
unsigned int IDLE_Data=8;

double CNBR_Para=3.50;
double VNBR_Para=2.00;

unsigned char PA7_Flag=0;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

struct Car
{
    char name[5];
    char type[5];
};

struct TIME
{
    int year;
    int month;
    int day;
    int hour;
    int min;
    int sec;
};

struct Parking
{
    struct Car car;
    struct TIME time;
    unsigned char park_flag;
};


struct Parking parking[9];
int outindex;

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
void led_proc(void);
void PA7_Proc(void);
void key_proc(void);
void usart_proc(void);

void Parking_init(struct Parking *park);
int IsParking(struct Parking *park,struct Car car,struct TIME time);
void OutParking(struct Parking *park,struct Car car,struct TIME time);
void InParking(struct Parking *park,struct Car car,struct TIME time);




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

    Parking_init(parking);
    
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
      key_proc();
      lcd_proc();
      PA7_Proc();
      led_proc();
      if(rx_pointer!=0)
      {
          int temp=rx_pointer;
          HAL_Delay(2);
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
        case 0://车位显示界面
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"       Data          ");
            LCD_DisplayStringLine(Line1,lcd_text);

            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"   CNBR:%d           ",CNBR_Data);
            LCD_DisplayStringLine(Line3,lcd_text);
        
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"   VNBR:%d           ",VNBR_Data);
            LCD_DisplayStringLine(Line5,lcd_text);
        
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"   IDLE:%d           ",IDLE_Data);
            LCD_DisplayStringLine(Line7,lcd_text);
        
            break;
        case 1://费率设置界面
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"       Para          ");
            LCD_DisplayStringLine(Line1,lcd_text);

            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"   CNBR:%0.2f           ",CNBR_Para);
            LCD_DisplayStringLine(Line3,lcd_text);
        
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"   VNBR:%0.2f           ",VNBR_Para);
            LCD_DisplayStringLine(Line5,lcd_text);
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
    else if(1==key[1].single_flag)//CNBR、VNBR费率增加0.5元
    {
        key[1].single_flag=0;
        if(1==view)
        {
            CNBR_Para+=0.5;
            VNBR_Para+=0.5;
        }
    }
    else if(1==key[2].single_flag)//CNBR、VNBR费率减少0.5元
    {
        key[2].single_flag=0;
        if(1==view)
        {
           
            CNBR_Para-=0.5;
            VNBR_Para-=0.5;
            if(CNBR_Para<0)CNBR_Para=0;
            if(VNBR_Para<0)VNBR_Para=0;
        }
    }
    else if(1==key[3].single_flag)//切换PA7端口的输出状态
    {
        key[3].single_flag=0;
        PA7_Flag=!PA7_Flag;
    }
}

//初始化
void Parking_init(struct Parking *park)
{
    for(int i=0;i<8;i++)
    {
    park[i].park_flag=1;
    park[i].time.year=0;
    park[i].time.month=0;
    park[i].time.day=0;
    park[i].time.hour=0;
    park[i].time.min=0;
    park[i].time.sec=0;
    strcpy((char *)park[i].car.name,"");
    strcpy((char *)park[i].car.type,"");
        
    }
}

//判断是停入停车场还是退出停车场
int IsParking(struct Parking *park,struct Car car,struct TIME time)
{
    int i;
    for(i=0;i<8;i++)
    {
        if((0==strcmp(park[i].car.type,car.type))&&(0==strcmp(park[i].car.name,car.name)))
        {
            outindex=i;
            return 1;
        }
    }
    
    //返回0是进入停车场
    return 0;
}

//退出停车场，并在串口输出计费信息
void OutParking(struct Parking *park,struct Car car,struct TIME time)
{
    double value;
    int hours;
    
    hours=(time.year-park[outindex].time.year)*356*24+(time.month-park[outindex].time.month)*30*24+(time.day-park[outindex].time.day)*24+(time.hour-park[outindex].time.hour);
    if((time.min!=park[outindex].time.min)||(park[outindex].time.sec!=time.sec))
    {
        hours=hours+1;
    }
    if(0==strcmp(car.type,"CNBR"))
    {
        value=1.0*hours*CNBR_Para;
        CNBR_Data--;
    }
    else if(0==strcmp(car.type,"VNBR"))
    {
        value=1.0*hours*VNBR_Para;
        VNBR_Data--;
    }
    
    park[outindex].park_flag=1;
    park[outindex].time.year=0;
    park[outindex].time.month=0;
    park[outindex].time.day=0;
    park[outindex].time.hour=0;
    park[outindex].time.min=0;
    park[outindex].time.sec=0;
    strcpy((char *)park[outindex].car.name,"");
    strcpy((char *)park[outindex].car.type,"");
   
    IDLE_Data++;
    
    memset(usart_text,0,sizeof(usart_text));
    sprintf((char *)usart_text,"%s:%s:%d:%0.2f",car.type,car.name,hours,value);
    HAL_UART_Transmit(&huart1,usart_text,30,50);
}

//进入停车场
void InParking(struct Parking *park,struct Car car,struct TIME time)
{
    //停车场满了
    if(IDLE_Data<=0)return;
    
    for(int i=0;i<8;i++)
    {
        //可以停
        if(1==park[i].park_flag)
        {
            if(0==strcmp(car.type,"CNBR"))
            {
                CNBR_Data++;
            }
            else if(0==strcmp(car.type,"VNBR"))
            {
                VNBR_Data++;
            }
            
            IDLE_Data--;
            
            park[i].park_flag=0;
            park[i].time.year=time.year;
            park[i].time.month=time.month;
            park[i].time.day=time.day;
            park[i].time.hour=time.hour;
            park[i].time.min=time.min;
            park[i].time.sec=time.sec;
            strcpy(park[i].car.type,car.type);
            strcpy(park[i].car.name,car.name);
            break;
        }
    }
    
}

void usart_proc(void)
{
   if(rx_pointer!=0)
   {
       if(22==rx_pointer)
       {
           char cartim[13];
          struct Car ca;
          struct TIME tim;
          sscanf((char *)rxdata,"%4s:%4s:%12s",ca.type,ca.name,cartim);
          if((0==strcmp(ca.type,"CNBR"))||(0==strcmp(ca.type,"VNBR")))
          {
              tim.year=(cartim[0]-'0')*10+(cartim[1]-'0')+2000;
              tim.month=(cartim[2]-'0')*10+(cartim[4]-'0');
              tim.day=(cartim[4]-'0')*10+(cartim[5]-'0');
              tim.hour=(cartim[6]-'0')*10+(cartim[7]-'0');
              tim.min=(cartim[8]-'0')*10+(cartim[9]-'0');
              tim.sec=(cartim[10]-'0')*10+(cartim[11]-'0');
          
               //退出停车场
              if(1==IsParking(parking,ca,tim))
              {
                  OutParking(parking,ca,tim);
              }
              //进入停车场
              else if(0==IsParking(parking,ca,tim))
              {
                  InParking(parking,ca,tim);
              }
          }
          else
          {
           memset(usart_text,0,sizeof(usart_text));
           sprintf((char *)usart_text,"Error");
           HAL_UART_Transmit(&huart1,usart_text,5,50);              
          }
        
       }
       else
       {
           memset(usart_text,0,sizeof(usart_text));
           sprintf((char *)usart_text,"Error");
           HAL_UART_Transmit(&huart1,usart_text,5,50);
       }
       
       rx_pointer=0;
       memset(rxdata,0,sizeof(rxdata));
   }       
}

void PA7_Proc(void)
{
    //2KHZ,低电平
    if(0==PA7_Flag)
    {
        __HAL_TIM_SetCompare(&htim17,TIM_CHANNEL_1,0);
    }
    //2KHZ,20%占空比
    else
    {
        __HAL_TIM_SetCompare(&htim17,TIM_CHANNEL_1,20);
    }
}

void led_proc(void)
{
    if((uwTick-uwTick_Led)<50)return;
    uwTick_Led=uwTick;
    
    //存在空闲车位，LD1亮
    if(IDLE_Data>0)
    {
        ucled|=0x01;
    }
    else
    {
        ucled&=0xfe;
    }
    //PA7输出2KHZ,20%占空比脉冲信号期间，LD2点亮
    if(1==PA7_Flag)
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
