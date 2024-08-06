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
#include "ds18b20_hal.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

unsigned char ucled=0x00;
unsigned char lcd_text[30];
unsigned char view;

__IO uint32_t uwTick_Lcd;
__IO uint32_t uwTick_Led;
__IO uint32_t uwTick_Review;

double A;
double T;

int FH=2000;
double AH=3.0;
int TH=30;

int FH_real=2000;
double AH_real=3.0;
int TH_real=30;

int FN=0;
int AN=0;
int TN=0;

int FP=1;
double VP=0.9;
int TT=6;

int FP_real=1;
double VP_real=0.9;
int TT_real=6;

unsigned char change_fset;
unsigned char change_para;

unsigned char review=0;
//unsigned int review_sec=0;

unsigned int review_index;
unsigned char review_flag=0;

double store_F[1000];
double store_D[1000];
double store_A[1000];

unsigned char reset;

//记录信号
unsigned char storing=0;

extern unsigned int reviewA_sec;
extern unsigned int reviewF_sec;

unsigned char store_flag=0;
unsigned int store_index=0;
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
void review_proc(void);
void store_proc(void);

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
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */
    
    led_disp(ucled);
    
    LCD_Init();
    LCD_Clear(Black);
    LCD_SetBackColor(Black);
    LCD_SetTextColor(White);
    
    HAL_TIM_Base_Start_IT(&htim4);
    HAL_TIM_Base_Start_IT(&htim6);
    
    HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
      key_proc();
      store_proc();
      review_proc();
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

void store_proc(void)
{
    if(1==storing)
    { 
        store_F[store_index]=pa1_frq;
        store_D[store_index]=pa1_duty;
        store_A[store_index]=A;
       // store_flag=0;
    }

}

void review_proc(void)
{
//    if((uwTick-uwTick_Review)<100)return;
    unsigned int pre=0;
    double duty=0;

    if(1==review){
        //review_flag=0;
            //脉冲回放
            //频率=（80000000/预分频系数）/重装载值
            //占空比=比较值/重装载值*100；
            if(1==review_flag)
            {
                review_flag=0;
                review_index++;
            }
            pre=(80000000.0/100)/(store_F[review_index]/FP_real)-1;
            duty=store_D[review_index];
            __HAL_TIM_SET_PRESCALER(&htim3,pre);
            __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_2,duty);
            HAL_TIM_GenerateEvent(&htim3,TIM_EVENTSOURCE_UPDATE);
    }
    if(2==review)
    {
        //review_flag=0;
        pre=80000000/100/1000-1;
        //频率固定为1KHZ
        if(2==review_flag)
         {
                review_flag=0;
                review_index++;
         }
        __HAL_TIM_SET_PRESCALER(&htim3,pre);
        HAL_TIM_GenerateEvent(&htim3,TIM_EVENTSOURCE_UPDATE);
        
        if(store_A[review_index]<=VP_real)
        {
            duty=10.0;
        }
        else if(store_A[review_index]>=3.3)
        {
            duty=100.0;
        }
        else
        {
            duty=(90.0/(3.3-VP_real))*store_A[review_index]+(10.0-90.0*VP_real/(3.3-VP_real));
        }
        __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_2,duty);    
    }
    
    if(0==view)
    {
      review_index=0;  
    }

   
}

void key_proc(void)
{
    if(0==storing){
        if(1==key[0].single_flag)
        {
            key[0].single_flag=0;
            //实时数据界面到报警界面，当前可调整的报警参数是频率上限参数
            if(0==view)change_para=0;
            //报警统计界面进入到回放设置界面，当前可调整的是脉冲信号回放分频系数
            if(2==view)change_fset=0;
            
            if(1==view)//参数设置界面退出时新参数生效
            {
                FH_real=FH;
                AH_real=AH;
                TH_real=TH;
            }
            
            if(3==view)//回放设置界面退出时新参数生效
            {
                FP_real=FP;
                VP_real=VP;
                TT_real=TT;
            }
            
            view=(view+1)%4;
            LCD_Clear(Black);
            
        }
        else if(1==key[1].single_flag)
        {
            key[1].single_flag=0;
            switch(view)
            {
                case 0://实时数据界面下，记录电压和PA引脚的脉冲信号频率、占空比
                   storing=1;
                    memset(store_A,0,sizeof(store_A));
                    memset(store_D,0,sizeof(store_D));
                    memset(store_F,0,sizeof(store_F));
                   store_index=0;
                    break;
                case 1://报警参数界面下，选择修改的参数，从报警界面退出时新的参数生效
                    change_para=(change_para+1)%3;
                    break;
                case 2://报警统计界面下，清零频率、电压和温度报警统计次数
                    FN=0;
                    AN=0;
                    TN=0;
                    break;
                case 3://回放设置界面下，切换选择修改的回放参数，从回放设置界面退出时，新的参数生效
                    change_fset=(change_fset+1)%3;
                    break;
            }
        }
        else if((1==key[2].long_flag)&&(1==key[3].long_flag))
        {
            key[2].long_flag=0;
            key[3].long_flag=0; 
            key[2].single_flag=0;
            key[3].single_flag=0;
            reset=1;             
        }
        else if(1==key[2].single_flag)
        {
            key[2].single_flag=0;
            key[2].long_flag=0;
            switch(view)
            {
                case 0://实时数据界面下，若完成了数据记录，则通过PA7引脚回放电压信号
                    if(store_index!=0){
                      //  ucled|=0x80;
                     review=2;
                     review_index=0;
                     reviewA_sec=0;
                    }
                    break;
                case 1://报警参数界面下，加
                    switch(change_para)
                    {
                        case 0://修改频率上限（FH）
                            FH+=1000;
                            if(FH==11000)
                            {
                                FH=10000;
                            }
                            break;
                        case 1://修改电压上限（AH）
                            AH+=0.3;
                            if(AH>3.3)
                            {
                                AH=3.3;
                            }
                            break;
                        case 2://修改温度上限（TH）
                            TH+=1;
                            if(TH>80)
                            {
                                TH=80;
                            }
                            break;
                    }
                    break;
                case 3://回放设置界面下，加
                    switch(change_fset)
                    {
                        case 0://修改脉冲回放分频系数（FP）
                            FP+=1;
                            if(FP>10)
                            {
                                FP=10;
                            }
                            break;
                        case 1://修改电压回放最小值（VP）
                            VP+=0.3;
                            if(VP>3.3)
                            {
                                VP=3.3;
                            }
                            break;
                        case 2://修改记录回放时间（TT）
                            TT+=2;
                            if(TT>10)
                            {
                                TT=10;
                            }
                            break;
                    }
                    break;
            }
        }
        else if(1==key[3].single_flag)
        {
            key[3].single_flag=0;
            key[3].long_flag=0;
            switch(view)
            {
               case 0://实时数据界面下，若完成了数据记录，则通过PA7引脚回放脉冲信号
                   if(store_index!=0){
                     review=1;
                     review_index=0;
                     reviewF_sec=0;
                   }
                     break;
                case 1://报警参数界面下，加
                    switch(change_para)
                    {
                        case 0://修改频率上限（FH）
                            FH-=1000;
                            if(FH<1000)
                            {
                                FH=1000;
                            }
                            break;
                        case 1://修改电压上限（AH）
                            AH-=0.3;
                            if(AH<0)
                            {
                                AH=0;
                            }
                            break;
                        case 2://修改温度上限（TH）
                            TH-=1;
                            if(TH<0)
                            {
                                TH=0;
                            }
                            break;
                    }
                    break;
                case 3://回放设置界面下，加
                    switch(change_fset)
                    {
                        case 0://修改脉冲回放分频系数（FP）
                            FP-=1;
                            if(FP<1)
                            {
                                FP=1;
                            }
                            break;
                        case 1://修改电压回放最小值（VP）
                            VP-=0.3;
                            if(VP<0)
                            {
                                VP=0;
                            }
                            break;
                        case 2://修改记录回放时间（TT）
                            TT-=2;
                            if(TT<2)
                            {
                                TT=2;
                            }
                            break;
                    }
                    break; 
            }
        } 
    } 
    else
    {
        key[0].single_flag=0;
        key[0].long_flag=0;
        key[1].single_flag=0;
        key[1].long_flag=0;
        key[2].single_flag=0;
        key[2].long_flag=0;
        key[3].single_flag=0;
        key[3].long_flag=0;
    }
    if(reset==1)
    {
            reset=0;
        
            AH=3.0;
            FH=2000;
            TH=30;
            FP=1;
            VP=0.9; 
            TT=6;
            FN=0;
            AN=0;
            TN=0;
            ucled=0x00;
            view=0;
            
            FP_real=1;
            VP_real=0.9;
            TT_real=6;
            FH_real=2000;
            AH_real=3.0;
            TH_real=30;
    }
}

void lcd_proc(void)
{
    if((uwTick-uwTick_Lcd)<100)return;
    uwTick_Lcd=uwTick;
    
    A=Get_ADC(&hadc2);
    T=temperature_read();
    
    switch(view)
    {
        case 0://实时数据界面
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"        DATA         ");
            LCD_DisplayStringLine(Line1,lcd_text);
        
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     F=%d                 ",pa1_frq);
            LCD_DisplayStringLine(Line3,lcd_text);
        
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     D=%0.0f%%             ",pa1_duty);
            LCD_DisplayStringLine(Line4,lcd_text);

            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     A=%0.1f             ",A);
            LCD_DisplayStringLine(Line5,lcd_text);

            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     T=%0.1f            ",T);
            LCD_DisplayStringLine(Line6,lcd_text);

            break;
        case 1://报警参数界面
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"        PARA         ");
            LCD_DisplayStringLine(Line1,lcd_text);
        
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     FH=%d         ",FH);
            LCD_DisplayStringLine(Line3,lcd_text);
        
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     AH=%0.1f      ",AH);
            LCD_DisplayStringLine(Line4,lcd_text);
           
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     TH=%d      ",TH);
            LCD_DisplayStringLine(Line5,lcd_text);
            break;
        case 2://报警统计界面
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"        RECD         ");
            LCD_DisplayStringLine(Line1,lcd_text);
        
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     FN=%d         ",FN);
            LCD_DisplayStringLine(Line3,lcd_text);
        
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     AN=%d         ",AN);
            LCD_DisplayStringLine(Line4,lcd_text);
        
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     TN=%d         ",TN);
            LCD_DisplayStringLine(Line5,lcd_text);
        
            break;
        case 3://回放设置界面
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"        FSET         ");
            LCD_DisplayStringLine(Line1,lcd_text);
        
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     FP=%d         ",FP);
            LCD_DisplayStringLine(Line3,lcd_text);
        
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     VP=%0.1f      ",VP);
            LCD_DisplayStringLine(Line4,lcd_text);
           
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     TT=%d      ",TT);
            LCD_DisplayStringLine(Line5,lcd_text);
            break;
    }
    
}

void led_proc(void)
{
    if((uwTick-uwTick_Led)<50)return;
    uwTick_Led=uwTick;
    
    if(pa1_frq>FH_real)
    {
        //如果原本LD4是熄灭状态
        if((ucled&0x08)==0)
        {
            FN++;
        }
        ucled|=0x08;
    }
    else
    {
        ucled&=0xf7;
    }
    
    if(A>AH_real)
    {
        //如果LD5是熄灭状态
        if((ucled&0x10)==0)
        {
            AN++;
        }
        ucled|=0x10;
    }
    else
    {
        ucled&=0xef;
    }
    
    if(T>TH_real)
    {
        //如果LD6是熄灭状态
        if((ucled&0x20)==0)
        {
            TN++;
         
        }
        ucled|=0x20;
    }
    else
    {
        ucled&=0xdf;
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
