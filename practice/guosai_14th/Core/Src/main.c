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
#include "myadc.h"
#include "timer.h"
#include "lcd.h"
#include "ds18b20_hal.h"


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

unsigned char ucled=0x00;
unsigned char lcd_text[30];

__IO uint32_t uwTick_Lcd;
__IO uint32_t uwTick_Led;

unsigned char view=0;

double A_r37=0;//R37的电压
double T=0;//采集到的温度值

int FH=2000;//频率上限
double AH=3.0;//电压上限
int TH=30;//温度上限

unsigned int FN=0;//频率报警次数
unsigned int AN=0;//电压报警次数
unsigned int TN=0;//温度报警次数

int FP=1;//脉冲回放分频系数
double VP=0.9;//电压信号回放最小值
int TT=6;//记录回放时间

unsigned char lock=0;//锁
unsigned char choose_temp_1;//报警参数界面选择需要修改的参数
unsigned char choose_temp_3;//回放设置选择需要修改的参数

unsigned char signal=0;//选择回放哪个

//记录信号
double store_r37;
unsigned int store_frq;
int store_t;//记录时长
double store_duty;

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
void signal_pre(void);


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
    
    HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_2);
    
    HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2);
    
    //等待DS18B20温度转换完成，防止插拔接口时产生的误读
    while(((unsigned int)re_temperature())==85.0);
    
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
      signal_pre();
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
    
    A_r37=GetAdc(&hadc2);
    T=re_temperature();
    
    switch(view)
    {
        case 0://实时数据界面
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"        DATA       ");
            LCD_DisplayStringLine(Line1,lcd_text);
        
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     F=%d        ",frq);
            LCD_DisplayStringLine(Line3,lcd_text);
        
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     D=%0.0f%%     ",duty);
            LCD_DisplayStringLine(Line4,lcd_text);
 
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     A=%0.1f     ",A_r37);
            LCD_DisplayStringLine(Line5,lcd_text);

            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     T=%0.1f     ",T);
            LCD_DisplayStringLine(Line6,lcd_text);
        
            break;
        case 1://报警参数界面
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"        PARA        ");
            LCD_DisplayStringLine(Line1,lcd_text);
        
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     FH=%d        ",FH);
            LCD_DisplayStringLine(Line3,lcd_text);
        
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     AH=%0.1f     ",AH);
            LCD_DisplayStringLine(Line4,lcd_text);
 
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     TH=%d     ",TH);
            LCD_DisplayStringLine(Line5,lcd_text);
            
            break;
        case 2://报警统计界面
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"        RECD     ");
            LCD_DisplayStringLine(Line1,lcd_text);
        
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     FN=%d        ",FN);
            LCD_DisplayStringLine(Line3,lcd_text);
        
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     AN=%d      ",AN);
            LCD_DisplayStringLine(Line4,lcd_text);
 
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     TN=%d      ",TN);
            LCD_DisplayStringLine(Line5,lcd_text);

            break;
        case 3://回放设置界面
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"        FSET     ");
            LCD_DisplayStringLine(Line1,lcd_text);
        
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     FP=%d        ",FP);
            LCD_DisplayStringLine(Line3,lcd_text);
        
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     VP=%0.1f     ",VP);
            LCD_DisplayStringLine(Line4,lcd_text);
 
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     TT=%d     ",TT);
            LCD_DisplayStringLine(Line5,lcd_text);

            break;
    }
}

void key_proc(void)
{
    if((1==key[0].single_flag)&&(0==lock))//界面切换按键
    {
        key[0].single_flag=0;
        if(0==view)choose_temp_1=0;
        if(2==view)choose_temp_3=0;
        view++;
        if(4==view)view=0;
        
        LCD_Clear(Black);
    }
    if((1==key[1].single_flag)&&(0==lock))
    {
        key[1].single_flag=0;
        if(0==view)//实时数据界面下，记录R37电压和PA1引脚的频率、占空比，记录完成前，其他按键失效
        {
            lock=1;
            store_t=TT;
        }
        else if(1==view)//报警参数界面下
        {
            choose_temp_1++;
            if(3==choose_temp_1)choose_temp_1=0;
        }
        else if(3==view)//回放设置界面下
        {
            choose_temp_3++;
            if(3==choose_temp_3)choose_temp_3=0;
        }
        else if(2==view)//报警统计界面下，清零频率、电压和温度报警统计次数
        {
            FN=0;
            AN=0;
            TN=0;
        }
    }
    if((0==lock)&&(1==key[2].long_flag)&&(1==key[3].long_flag))
    {
        key[2].long_flag=0;key[3].long_flag=0;

            view=0;
            FH=2000;
            AH=3.0;
            TH=30;
            FP=1;
            VP=0.9;
            TT=6;
            FN=0;
            AN=0;
            TN=0;        
    }
    if((1==key[2].single_flag)&&(0==lock))//加按键
    {
        key[2].single_flag=0;
        if(1==view)//报警参数界面下
        {
            switch(choose_temp_1)
            {
                case 0://选择频率上限参数(FH),FH值加1000HZ，可调整范围是1KHZ-10KHZ
                    FH+=1000;
                    if(FH>=11000)FH=1000;
                    break;
                case 1://选择电压上限参数(AH),AH值加0.3V，可调整范围：0V-3.3V
                    AH+=0.3;
                    if(AH>3.3)AH=0;
                    break;
                case 2://选择温度上限参数(TH)，TH值加1°C，可调整范围：0°C-80°C
                    TH+=1;
                    if(TH>80)TH=0;
                    break;
            }
        }
        else if(3==view)//设置回放界面下
        {
            switch(choose_temp_3)
            {
                case 0://选择脉冲信号回放分频参数(FP)，FP值加1，可调整范围：1-10
                    FP+=1;
                    if(FP>=11)FP=1;
                    break;
                case 1://选择电压信号回放最小值(VP)，VP值加0.3V，可调整范围：0V-3.3V
                    VP+=0.3;
                    if(VP>3.3)VP=0;
                    break;
                case 2://选择记录回放时间（TT）,TT值加2秒，可调整范围：2秒-10秒
                    TT+=2;
                    if(TT>10)TT=2;
                    break;
            }
        }
        else if(0==view)//实时数据界面下
        {
            //回放电压信号
            signal=1;
        }
    }
    if((1==key[3].single_flag)&&(0==lock))//减按键
    {
        key[3].single_flag=0;
        if(1==view)//报警参数界面下
        {
            switch(choose_temp_1)
            {
                case 0://选择频率上限参数(FH),FH值减1000HZ，可调整范围是1KHZ-10KHZ
                    FH-=1000;
                    if(FH<=0)FH=10000;
                    break;
                case 1://选择电压上限参数(AH),AH值减0.3V,可调整范围：0V-3.3V
                    AH-=0.3;
                    if(AH<0)AH=3.3;
                    break;
                case 2://选择温度上限参数(TH)，TH值减1°C,可调整范围：0°C-80°C
                    TH-=1;
                    if(TH<0)TH=80;
                    break;
            }
        }
        else if(3==view)//设置回放界面下
        {
            switch(choose_temp_3)
            {
                case 0://选择脉冲信号回放分频参数(FP)，FP值减1，可调整范围：1-10
                    FP-=1;
                    if(FP<1)FP=10;
                    break;
                case 1://选择电压信号回放最小值(VP)，VP值减0.3V，可调整范围：0V-3.3V
                    VP-=0.3;
                    if(VP<0)VP=3.3;
                    break;
                case 2://选择记录回放时间（TT）,TT值减2秒,可调整范围：2秒-10秒
                    TT-=2;
                    if(TT<2)TT=10;
                    break;
            }
        }
        else if(0==view)//实时数据界面下
        {
            //回放脉冲信号
            signal=2;
        }
    }  
}

void led_proc(void)
{
    if((uwTick-uwTick_Led)<50)return;
    uwTick_Led=uwTick;
    
    if(frq>FH)//LD4点亮
    {
        ucled|=0x08;
    }
    else
    {
        ucled&=0xf7;
    }
    
    if(A_r37>AH)//LD5点亮
    {
        ucled|=0x10;
    }
    else
    {
        ucled&=0xef;
    }
    
    if(T>TH)//LD6点亮
    {
        ucled|=0x20;
    }
    else
    {
        ucled&=0xdf;
    }
    
    led_disp(ucled);
    
}

//信号回放
void signal_pre(void)
{
    //电压信号回放
    //PA7输出频率固定为1KHZ
    if(signal==1){
        //改变重装载值使得PA7的输出频率固定为1KHZ
        __HAL_TIM_SET_PRESCALER(&htim3,8000000/100/1000);
        
        if(A_r37<=VP)//占空比：10%
        {
            //占空比=比较值/重装载值*100
            __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_2,(0.1*(TIM3->ARR+1)));
        }
        else if(A_r37>=3.3)//占空比：100%
        {
            __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_2,(1.0*(TIM3->ARR+1)));
        }
        else
        {
            double value=(90.0/(3.3-VP))*A_r37+(33-80*VP)/(3.3-VP);
            __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_2,(value*(TIM3->ARR+1))/100.0);
        }
    }
    else if(signal==2)//脉冲信号回放
    {
        //将记录下来的PA1上频率和占空比通过PA7播放输出
        //输出信号频率按照FP进行分频处理
        //输出频率按照FP进行分频处理
        //占空比与记录值保持一致
        __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_2,store_duty/100.0*(store_frq));
        //该函数可以直接更改Prescaler(PSC)预分频寄存器的数值
        //频率=80MHZ/预分频系数/重装载值
        //则：预分频系数=80MHZ/重装载值/频率
        __HAL_TIM_SET_PRESCALER(&htim3,(80000000/100)/store_frq/FP);
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
