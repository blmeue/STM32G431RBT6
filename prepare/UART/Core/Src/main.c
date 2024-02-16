/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "led.h"
#include "lcd.h"
#include "stdio.h"
#include "interrupt.h"
#include "badc.h"
#include "i2c_hal.h"
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
extern struct keys key[];//在main函数引用interrupt.c文件里的struct keys key变量
extern uint frq1,frq2;
extern float duty1,duty2;

uchar view=0;//默认在第一个界面

uchar pa6_duty=10;//PA6的占空比
uchar pa7_duty=10;

extern char rxdata[30];//最终处理好的数据
extern uint8_t rxdat;//每次接收的数据
extern uchar rx_pointer;//确定一下写到哪个位置了
char car_type[5];
char car_data[5];
char car_time[13];


void key_proc(void);
void disp_proc(void);
void uart_rx_porc();

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
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM16_Init();
  MX_TIM17_Init();
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
    LED_Disp(0x00);//LED的初始化，即让所有led灭
    LCD_Init();//LCD屏的初始化
    
    //初始化串口
    HAL_UART_Receive_IT(&huart1,&rxdat,1);//每次中断只能接收一个字符

	LCD_Clear(Black);//把屏幕清空，清成黑色
	LCD_SetBackColor(Black);//设置背景色
	LCD_SetTextColor(White);//设置文本颜色(前景色)
    
	HAL_TIM_Base_Start_IT(&htim4);//开启定时器3的中断
    
    //PA6输出频率固定为100HZ，占空比可调节的脉冲信号
    //PA7输出频率固定为200HZ，占空比可调节的脉冲信号
    //打开PWM输出
    HAL_TIM_PWM_Start(&htim16,TIM_CHANNEL_1);//第一个参数为哪个定时器，第二个参数为通道几
    HAL_TIM_PWM_Start(&htim17,TIM_CHANNEL_1);//第一个参数为哪个定时器，第二个参数为通道几
    HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_1);//打开定时器2通道1
    HAL_TIM_IC_Start_IT(&htim3,TIM_CHANNEL_1);//打开定时器3通道1
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
//      LED_Disp(0x01);
//      HAL_Delay(500);//延时500ms
//      LED_Disp(0x00);
//      HAL_Delay(500);//延时500ms
      
      key_proc();
      disp_proc();
      //防止接收不完整
      if(rx_pointer!=0){
      int temp=rx_pointer;
          HAL_Delay(1);
          if(temp==rx_pointer)uart_rx_porc();
      }
      
//      char temp[20];
//      //将频率发送出去,发送到串口助手上
//      sprintf(temp,"frq=%d\n",frq1);
//      //第一个参数是哪一个串口，第二个参数是发送的数据
//      //第三个参数是要发送的长度
//      //第四个参数是超时时间
//      HAL_UART_Transmit(&huart1,(uint8_t *)temp,strlen(temp),50);
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 10;
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
void key_proc(void){
      if(key[0].single_flag==1){//第一个按键被按下
      view++;
       if(view==3)view=0;
      LCD_Clear(Black);//切换界面时要进行清屏操作，避免上一界面残留
      key[0].single_flag=0;//将按下标志位清0，避免被重复执行  
      }
      if(key[1].single_flag==1){//第二个按键被按下
      pa6_duty+=10;
      if(pa6_duty>=100)
          pa6_duty=10;
       //将设置的占空比应用到输出当中
      __HAL_TIM_SetCompare(&htim16,TIM_CHANNEL_1,pa6_duty);//第一个参数是哪个定时器，第二个参数是哪个通道，第三个参数是值
      LCD_Clear(Black);//切换界面时要进行清屏操作，避免上一界面残留
      key[1].single_flag=0;//将按下标志位清0，避免被重复执行  
          
      }  
      if(key[2].single_flag==1){//第三个按键被按下
      pa7_duty+=10;
      if(pa7_duty>=100)
          pa7_duty=10;
      __HAL_TIM_SetCompare(&htim17,TIM_CHANNEL_1,pa7_duty);//第一个参数是哪个定时器，第二个参数是哪个通道，第三个参数是值
      LCD_Clear(Black);//切换界面时要进行清屏操作，避免上一界面残留
      key[2].single_flag=0;//将按下标志位清0，避免被重复执行  
          
      }  
      if(key[3].single_flag==1){//第四个按键被按下
            //将频率存储到EEPROM
          //EEPROM是8位，频率是16位，所以需要把频率分为高八位和低八位
         uchar frq_h=frq1>>8;
          uchar frq_l=frq1&0xff;
          eeprom_write(1,frq_h);
          HAL_Delay(10);//延时10毫秒
          eeprom_write(2,frq_l);
          key[3].single_flag=0;//将按下标志位清0，避免被重复执行  
          
      }        
}

//LCD屏显示
void disp_proc(void){
    if(view==0){
      char text[30];//字符数组
      sprintf(text,"      Data       ");
      LCD_DisplayStringLine(Line1, (uint8_t *)text);//将text里面的数据打印到第1行
        
      sprintf(text,"      FRQ1=%d       ",frq1);
      LCD_DisplayStringLine(Line2, (uint8_t *)text);//将text里面的数据打印到第2行
      sprintf(text,"      duty1=%.3f       ",duty1);
      LCD_DisplayStringLine(Line3, (uint8_t *)text);//将text里面的数据打印到第3行

     sprintf(text,"      FRQ2=%d       ",frq2);
     LCD_DisplayStringLine(Line4, (uint8_t *)text);//将text里面的数据打印到第4行
     sprintf(text,"      duty2=%.3f       ",duty2);
     LCD_DisplayStringLine(Line5, (uint8_t *)text);//将text里面的数据打印到第5行  
         
        sprintf(text,"      V:%.2f v      ",getADC(&hadc1));
     LCD_DisplayStringLine(Line6, (uint8_t *)text);//将text里面的数据打印到第6行  
     
        sprintf(text,"      V:%.2f v     ",getADC(&hadc2));
     LCD_DisplayStringLine(Line7, (uint8_t *)text);//将text里面的数据打印到第7行  
            
     uint eep_temp=(eeprom_read(1)<<8)+(eeprom_read(2));//对读出来的数据进行处理
     
     sprintf(text,"      FRQ_eep=%d     ",eep_temp);
     LCD_DisplayStringLine(Line8, (uint8_t *)text);//将text里面的数据打印到第7行  
        
    }
    if(view==1){
      char text[30];//字符数组
      sprintf(text,"      Para       ");
      LCD_DisplayStringLine(Line1, (uint8_t *)text);//将text里面的数据打印到第九行
        sprintf(text,"    PA6:%d       ",pa6_duty);
        LCD_DisplayStringLine(Line2, (uint8_t *)text);//将text里面的数据打印到第九行
        sprintf(text,"    PA7:%d       ",pa7_duty);
        LCD_DisplayStringLine(Line4, (uint8_t *)text);//将text里面的数据打印到第九行
 
    }
    if(view==2){
      char text[30];//字符数组
      sprintf(text,"      Car_msg       ");
        
      LCD_DisplayStringLine(Line1, (uint8_t *)text);//将text里面的数据打印到第九行
        sprintf(text,"car_type=%s",car_type);
      LCD_DisplayStringLine(Line2, (uint8_t *)text);//将text里面的数据打印到第九行
        sprintf(text,"car_data=%s",car_data);
      LCD_DisplayStringLine(Line3, (uint8_t *)text);//将text里面的数据打印到第九行
        sprintf(text,"car_tim=%s",car_time);
      LCD_DisplayStringLine(Line4, (uint8_t *)text);//将text里面的数据打印到第九行
        
    }
}

//接收函数：上位机发送
//接收：从电脑上的串口助手输入要发送的字符，单片机通过串口接收到
//发送：单片机通过串口发送数据到电脑上，并在串口助手上可以看到发送的数据信息
void uart_rx_porc()
{
    //判断是否有接收到东西
    if(rx_pointer>0)//接收到了
    {
        if(rx_pointer==22)//是否接收到了22个字符
        {
            //将rxdata拆分
            sscanf(rxdata,"%4s:%4s:%12s",car_type,car_data,car_time);
            
        }
        else//没有接收到22个字符，则发送Error
        {
      char temp[20];
      //将频率发送出去,发送到串口助手上
      sprintf(temp,"Error");
      //第一个参数是哪一个串口，第二个参数是发送的数据
      //第三个参数是要发送的长度
      //第四个参数是超时时间
      HAL_UART_Transmit(&huart1,(uint8_t *)temp,strlen(temp),50);           
        }
       rx_pointer=0;
       memset(rxdata,0,30);//将rxdata清空
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
