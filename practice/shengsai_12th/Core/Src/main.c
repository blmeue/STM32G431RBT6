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

#include "lcd.h"
#include "led.h"
#include "timer.h"
#include "myusart.h"
#include "stdio.h"
#include "String.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//汽车
struct Car
{
    char type[5];
    char id[5];
};
//时间
struct Time
{
    int year;
    int month;
    int day;
    int hour;
    int min;
    int sec;
};


//停车场
struct Parking
{
    struct Car car;
    struct Time time;
    int canpark;//判断能否停车
};
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
void uart_proc(void);
void led_proc(void);

int park(struct Parking *parking,struct Car car, struct Time time);
void parkinit(struct Parking *parking);
int Rename(struct Parking *parking,struct Car car, struct Time time);
void outParking(struct Parking *parking,struct Car car, struct Time time);
void ControlParking(struct Parking *parking,struct Car car, struct Time time);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

//led灯
unsigned char ucled=0x00;

//lcd上要显示的数据
unsigned char lcd_text[30];

//减速函数
__IO uint32_t uwTick_lcd;
__IO uint32_t uwTick_led;
__IO uint32_t uwTick_uart;

//显示界面参数
unsigned char view=0;

//空闲车位
unsigned int spare=8;

//CNBR类型车辆数量
unsigned int CNBR_num=0;
//CNBR类型停车费率
double CNBR_Park=3.5;

//VNBR类型车辆数量
unsigned int VNBR_num=0;
//VNBR类型停车费率
double VNBR_Park=2.0;

//按下B4的标志
unsigned char flag_B4=0;

int location=0;

struct Parking parking[9];


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
    
    //使能定时器4,开启定时器中断。
    HAL_TIM_Base_Start_IT(&htim4);
    
    //开启串口
    HAL_UART_Receive_IT(&huart1,&rxdat,1);
    
    //PWM
    HAL_TIM_PWM_Start(&htim17,TIM_CHANNEL_1);
    
    parkinit(parking);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
      key_proc();
      led_proc();
      lcd_proc();
      if(rx_pointer>0)
      {
          int temp=rx_pointer;
          HAL_Delay(1);
          if(temp==rx_pointer)
              uart_proc();
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

void key_proc(void)
{
    if(1==key[0].single_sta)
    {
        
        key[0].single_sta=0;
        view=(view+1)%2;
        LCD_Clear(Black);
        ucled|=0x01;
        led_disp(ucled);
    }
    else if(1==key[1].single_sta)//按下B2，VNBR、CNBR的费率增加0.5,仅在费率设置界面有效
    {
        key[1].single_sta=0;
        if(1==view)
        {
            VNBR_Park+=0.5;
            CNBR_Park+=0.5;
        }
    }
    else if(1==key[2].single_sta)//按下B3，VNBR、CNBR的费率减少0.5，仅在费率设置界面有效
    {
        key[2].single_sta=0;
        if(1==view)
        {
            VNBR_Park-=0.5;
            CNBR_Park-=0.5;
            
        }
    }
    else if(1==key[3].single_sta)//切换PA7的输出状态：（2KHZ、20%占空比的脉冲信号）或者低电平
    {
        key[3].single_sta=0;
        flag_B4=!flag_B4;
        
        if(0==flag_B4)//低电平
        {
            __HAL_TIM_SetCompare(&htim17,TIM_CHANNEL_1,0);
        }
        else if(1==flag_B4)//（2KHZ、20%占空比的脉冲信号）
        {
            
            __HAL_TIM_SetCompare(&htim17,TIM_CHANNEL_1,100);
        }
    }
}

void led_proc(void)
{
    if((uwTick-uwTick_led)<50)return;
    uwTick_led=uwTick;
      if(1==flag_B4)//（2KHZ、20%占空比的脉冲信号）
      {
          ucled|=0x02;
      }      
      else 
      {
          ucled&=0xfd;
      }
      
      if(spare>0)
      {
          ucled|=0x01;
      }
      else
      {
          ucled&=0xfe;
      }
      led_disp(ucled);
}

void parkinit(struct Parking *parking)
{
    for(int i=0;i<8;i++)
    {
        parking[i].canpark=1;
        parking[i].time.day=0;
        parking[i].time.month=0;
        parking[i].time.year=0;
        parking[i].time.hour=0;
        parking[i].time.min=0;
        parking[i].time.sec=0;
        strcpy(parking[i].car.id,"");
        strcpy(parking[i].car.type,"");
    }
}

int park(struct Parking *parking,struct Car car, struct Time time)
{
    int typeflag=0;
    if(1==Rename(parking,car,time))return 0;
    
    //没有位置了
    if(spare<=0)
    {
        return 0;
    }
    //判断车辆类型
    if(strcmp(car.type,"CNBR")==0)
    {
        typeflag=1;
    }
    else if(strcmp(car.type,"VNBR")==0)
    {
        typeflag=2;
    }
    int i;
    for(i=0;i<8;i++)
    {
        //判断是否能停车
        if(parking[i].canpark!=0)
        {
            if(typeflag==1)
            {
                CNBR_num++;
                spare--;
            }
            else if(typeflag==2)
            {
                VNBR_num++;
                spare--;
            }
			//到这就是成功停车了 结构体内赋值
			strcpy((parking[i].car).id, car.id);
			strcpy((parking[i].car).type, car.type);

			parking[i].time.year = time.year;
			parking[i].time.month = time.month;
			parking[i].time.day = time.day;
			parking[i].time.hour = time.hour;
			parking[i].time.min = time.min;
			parking[i].time.sec= time.sec;
            //该车位置为不能停车
			parking[i].canpark = 0;
     
			return 1;
            
        }
    }
    return 0;
}

int Rename(struct Parking *parking,struct Car car, struct Time time)
{
   int i;
   
    for(i=0;i<8;i++)
    {
       if((0==strcmp(car.id,parking[i].car.id))&&(0==strcmp(car.type,parking[i].car.type)))
       {
           location=i;
           return 1;
       }  
    }
   return 0;
    
}

void outParking(struct Parking *parking,struct Car car, struct Time time)  
{
    double money=0;
    double avg=0;
    
    int hours=0;
    
    if(0==strcmp(parking[location].car.type,"VNBR"))
    {
        VNBR_num--;
        avg=VNBR_Park;
    }
    else if(0==strcmp(parking[location].car.type,"CNBR"))
    {
        CNBR_num--;
        avg=CNBR_Park;
    }
    spare++;
    
    hours=(time.year-parking[location].time.year)*365*24+(time.month-parking[location].time.month)*30*24
            +(time.day-parking[location].time.day)*24+(time.hour-parking[location].time.hour+1);
    
    money=hours*avg;
    
    char str[30];
    sprintf(str,"%s:%s:%d:%0.2f",car.type,car.id,hours,money);
    HAL_UART_Transmit(&huart1,(uint8_t *)str,strlen(str),100);
    
        strcpy(parking[location].car.type,"");
		strcpy(parking[location].car.id,"");
		parking[location].time.year=0;
		parking[location].time.month=0;
		parking[location].time.day=0;
		parking[location].time.hour=0;
		parking[location].time.min=0;
		parking[location].time.sec=0;
		parking[location].canpark=1;   
}

void ControlParking(struct Parking *parking,struct Car car, struct Time time)
{
    if(0==park(parking,car,time))
    {
        outParking(parking,car,time);
    }
}

void uart_proc(void)
{
    if((uwTick-uwTick_uart)<100)return;
    uwTick_uart=uwTick;
    
    if(rx_pointer>0)
    {
             struct Car car;
            struct Time time;
        //获取车辆进出信息，并计算出费用
        if(22==rx_pointer)
        {

            char car_time[13];

            sscanf(rxdata,"%4s:%4s:%12s",car.type,car.id,car_time);
            
            time.year=2000+((int)car_time[0]-48)*10+((int)car_time[1]-48);
            time.month=((int)car_time[2]-48)*10+((int)car_time[3]-48);
            time.day=((int)car_time[4]-48)*10+((int)car_time[5]-48);
            time.hour=((int)car_time[6]-48)*10+((int)car_time[7]-48);
            time.min=((int)car_time[8]-48)*10+((int)car_time[9]-48);
            time.sec=((int)car_time[10]-48)*10+((int)car_time[11]-48);
           
            if((strcmp(car.type,"VNBR")!=0)&&(strcmp(car.type,"CNBR")!=0))
            {
            char temp[30];
            sprintf(temp,"Error Type Format\r\n");
            HAL_UART_Transmit(&huart1,(uint8_t *)temp,strlen(temp),50);
                   
            }
   
            ControlParking(parking,car,time);
        }
        else
        {
            char temp[30];
            sprintf(temp,"Error\r\n");
            HAL_UART_Transmit(&huart1,(uint8_t *)temp,strlen(temp),50);
        }
        Rename(parking,car,time);
        rx_pointer=0;
        memset(rxdata,0,30);
    }
}

void lcd_proc(void)
{
    if((uwTick-uwTick_lcd)<100)return;
    uwTick_lcd=uwTick;
    
    if(0==view)//车位显示界面
    {
       memset(lcd_text,0,sizeof(lcd_text));
       sprintf((char *)lcd_text,"       DATA             ");
       LCD_DisplayStringLine(Line1,lcd_text);

       memset(lcd_text,0,sizeof(lcd_text));
        sprintf((char *)lcd_text,"   CNBR:%d            ",CNBR_num);
       LCD_DisplayStringLine(Line3,lcd_text);

       memset(lcd_text,0,sizeof(lcd_text));
        sprintf((char *)lcd_text,"   VNBR:%d             ",VNBR_num);
       LCD_DisplayStringLine(Line5,lcd_text);

       memset(lcd_text,0,sizeof(lcd_text));
        sprintf((char *)lcd_text,"   IDLE:%d             ",spare);
       LCD_DisplayStringLine(Line7,lcd_text);
        
    }
    else if(1==view)//费率设置界面
    {
        memset(lcd_text,0,sizeof(lcd_text));
       sprintf((char *)lcd_text,"       PARA             ");
       LCD_DisplayStringLine(Line1,lcd_text);       

       memset(lcd_text,0,sizeof(lcd_text));
        sprintf((char *)lcd_text,"   CNBR:%0.2f             ",CNBR_Park);
       LCD_DisplayStringLine(Line3,lcd_text);

       memset(lcd_text,0,sizeof(lcd_text));
        sprintf((char *)lcd_text,"   VNBR:%0.2f             ",VNBR_Park);
       LCD_DisplayStringLine(Line5,lcd_text);
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
