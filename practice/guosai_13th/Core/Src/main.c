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
#include "i2c - hal.h"
#include "myadc.h"
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

unsigned char view;

double v_PA4;//P4��ѹ
double v_PA5;//PA5��ѹ
unsigned char display_PA45_flag=0;

unsigned char lcd_flag=0;

unsigned int x;//Ƶ�ʲ���
unsigned int y;//��ѹ����

unsigned int N4=0;//��ѹ���ݼ�¼����
double A4=0.0;//��ѹ���ֵ
double T4=0.0;//��ѹ��Сֵ
double H4=0.0;//ƽ��ֵ

unsigned int N5=0;//��ѹ���ݼ�¼����
double A5=0.0;//��ѹ���ֵ
double T5=0.0;//��ѹ��Сֵ
double H5=0;//ƽ��ֵ

double sum4=0;//��ֵ
double sum5=0;

unsigned char mode=0;
unsigned char LD3_flag=0;

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
void FRQ_PA7(void);
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
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
    
    LCD_Init();
    LCD_Clear(Black);
    LCD_SetBackColor(Black);
    LCD_SetTextColor(White);
    
    led_disp(ucled);
    
    HAL_TIM_Base_Start_IT(&htim4);
    
    HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2);
    
    HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_2);
    
    HAL_UART_Receive_IT(&huart1,&rxdat,1);
    
    
    if(eeprom_read(2)==0X67&&eeprom_read(3)==0X76)//���ǵ�һ���ϵ�
    {
            x=eeprom_read(1);//��EEPROM��ȡƵ�ʲ���
            y=eeprom_read(0);//��EEPROM��ȡ��ѹ����
    }
    else//��һ���ϵ�
    {
        eeprom_write(1,x);
        HAL_Delay(5);
        eeprom_write(0,y);
        HAL_Delay(5);
        eeprom_write(2,0x67);
        HAL_Delay(5);
        eeprom_write(3,0x76);
        HAL_Delay(5);
    }
   
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
      key_proc();
      if(rx_pointer!=0)
      {
          int temp=rx_pointer;
          HAL_Delay(2);
          if(temp==rx_pointer)usart_proc();
      }
      FRQ_PA7();
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

void lcd_proc(void)
{
    if((uwTick-uwTick_Lcd)<100)return;
    uwTick_Lcd=uwTick;

    switch(view)
    {
        case 0://���ݽ���
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"        DATA      ");
            LCD_DisplayStringLine(Line1,lcd_text);
        
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     PA4=%0.2f     ",v_PA4);
            LCD_DisplayStringLine(Line3,lcd_text);
            
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     PA5=%0.2f     ",v_PA5);
            LCD_DisplayStringLine(Line4,lcd_text);
        
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     PA1=%d     ",frq);
            LCD_DisplayStringLine(Line5,lcd_text);
        
            break;
        case 1://��������
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"        PARA      ");
            LCD_DisplayStringLine(Line1,lcd_text);
        
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     X=%d     ",x);
            LCD_DisplayStringLine(Line3,lcd_text);
            
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     Y=%d     ",y);
            LCD_DisplayStringLine(Line4,lcd_text);
            break;
        case 2://��¼����
            
           if(0==display_PA45_flag){
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"        REC-PA4      ");
            LCD_DisplayStringLine(Line1,lcd_text);
        
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     N=%d     ",N4);
            LCD_DisplayStringLine(Line3,lcd_text);
            
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     A=%0.2f     ",A4);
            LCD_DisplayStringLine(Line4,lcd_text);
        
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     T=%0.2f     ",T4);
            LCD_DisplayStringLine(Line5,lcd_text);
        
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     H=%0.2f     ",H4);
            LCD_DisplayStringLine(Line6,lcd_text);
           }
           else if(1==display_PA45_flag)
           {
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"        REC-PA5      ");
            LCD_DisplayStringLine(Line1,lcd_text);
        
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     N=%d     ",N5);
            LCD_DisplayStringLine(Line3,lcd_text);
            
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     A=%0.2f     ",A5);
            LCD_DisplayStringLine(Line4,lcd_text);
        
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     T=%0.2f     ",T5);
            LCD_DisplayStringLine(Line5,lcd_text);
        
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"     H=%0.2f     ",H5);
            LCD_DisplayStringLine(Line6,lcd_text);   
           }
        
            break;
    }
}

void key_proc(void)
{
    if(1==key[0].single_flag)//�л����水��
    {
        key[0].single_flag=0;
        view++;
        if(3==view)view=0;
        display_PA45_flag=0;
        LCD_Clear(Black);
    }
    else if(1==key[1].single_flag)//Ƶ�ʲ�����������
    {
        key[1].single_flag=0;
        if(1==view)//����������
        {
            x++;
            if(5==x)x=1;
        }            
    }
    else if(1==key[2].single_flag)//��ѹ������������
    {
        key[2].single_flag=0;
        if(1==view)
        {
            y++;
            if(5==y)y=1;
        }
    }
    
    if(1==key[3].single_flag)
    {
        key[3].single_flag=0;
        
        if(0==view)//����һ�ε�ѹ�������ܣ�LCD���ĵ�ѹ���ݸ���һ��
        { 
            v_PA4=GetAdc(&hadc2);
            v_PA5=GetAdc(&hadc2); 
            HAL_Delay(10);
            v_PA4=GetAdc(&hadc2);
            v_PA5=GetAdc(&hadc2); 
            
            //��¼��������
            N4++;
            N5++;
            
            //��ѹ���ֵ��A��
            
            if(v_PA4>A4)A4=v_PA4;
            if(v_PA5>A5)A5=v_PA5;
            
            //��ѹ��Сֵ��T��
            if(0==T4)T4=v_PA4;
            else if(v_PA4<T4)T4=v_PA4;
            if(0==T5)T5=v_PA5;
            else if(v_PA5<T5)T5=v_PA5;
            
            //ƽ��ֵ��H��
            sum4+=v_PA4;
            sum5+=v_PA5;
            
            H4=sum4/N4;
            H5=sum5/N5;
            
        }
        else if(1==view)//���������л��������ģʽΪ��Ƶģʽ���Ƶģʽ
        {
            mode=!mode;              
        }
        
        else if(2==view)//�����л���ͬ����ͨ���ļ�¼���
        {
            display_PA45_flag=!display_PA45_flag;
            LCD_Clear(Black);
        }
        
    }
     if(1==key[3].long_flag)//����1����ɿ������㵱ǰͨ����ȫ�����ݼ�¼���
    {
        key[3].long_flag=0;
        if(2==view){
        N4=0;//��ѹ���ݼ�¼����
        A4=0;//��ѹ���ֵ
        T4=0;//��ѹ��Сֵ
        H4=0;//ƽ��ֵ
        
        N5=0;//��ѹ���ݼ�¼����
        A5=0;//��ѹ���ֵ
        T5=0;//��ѹ��Сֵ
        H5=0;//ƽ��ֵ
        }
    }
    
    eeprom_write(0,y);//0��ַ�������ѹ����
    eeprom_write(1,x);//1��ַ������Ƶ�ʲ���
}

void FRQ_PA7(void)
{
     //ͨ�� PA7������������źţ�����ź�Ƶ�ʰ���Ƶ�ʲ������������ģʽ�������á�
    //FPA7 = FPA1 * Ƶ�ʲ���
    //FPA7 = FPA1 / Ƶ�ʲ���
    if(0==mode)//��Ƶģʽ
    {
        //Ƶ��=��80MHZ/Ԥ��Ƶϵ����/��װ��ֵ
        //����Ԥ��Ƶϵ��
        __HAL_TIM_SET_CAPTUREPOLARITY(&htim3,TIM_CHANNEL_2,(80000000)/(frq*x)/10000);
    }
    else if(1==mode)//��Ƶģʽ
    {
        //����Ԥ��Ƶϵ��
        __HAL_TIM_SET_CAPTUREPOLARITY(&htim3,TIM_CHANNEL_2,(80000000)/(frq/x)/10000);
    }
}

void usart_proc(void)
{
    if(rx_pointer!=0)
    {
        
        if(0==strcmp((char *)rxdata,"X"))//���ص�ǰƵ�ʲ�����
        {
            memset(usart_text,0,sizeof(usart_text));
            sprintf((char *)usart_text,"X:%d",x);
            HAL_UART_Transmit(&huart1,usart_text,3,50);
        }
        else if(0==strcmp((char *)rxdata,"Y"))//���ص�ǰ��ѹ������
        {
            memset(usart_text,0,sizeof(usart_text));
            sprintf((char *)usart_text,"Y:%d",y);
            HAL_UART_Transmit(&huart1,usart_text,3,50);
        }
        else if(0==strcmp((char *)rxdata,"PA1"))//���� PA1 ͨ��ʵʱ��������Ƶ�����ݡ�
        {
            memset(usart_text,0,sizeof(usart_text));
            sprintf((char *)usart_text,"PA1:%d",frq);
            HAL_UART_Transmit(&huart1,usart_text,10,50);
        }
        else if(0==strcmp((char *)rxdata,"PA4"))//���� PA4 ͨ����ǰ�������ĵ�ѹ���ݡ�
        {
            // double v_PA4v=GetAdc(&hadc2);
            // double v_PA5v=GetAdc(&hadc2);
            memset(usart_text,0,sizeof(usart_text));
            sprintf((char *)usart_text,"PA4:%0.2f",v_PA4);
            HAL_UART_Transmit(&huart1,usart_text,10,50);
        }
        else if(0==strcmp((char *)rxdata,"PA5"))//���� PA5 ͨ����ǰ�������ĵ�ѹ���ݡ�
        {
            //double v_PA4v=GetAdc(&hadc2);
            //double v_PA5v=GetAdc(&hadc2);
            memset(usart_text,0,sizeof(usart_text));
            sprintf((char *)usart_text,"PA5:%0.2f",v_PA5);
            HAL_UART_Transmit(&huart1,usart_text,10,50);
        }
        else if(0==strcmp((char *)rxdata,"#"))//�л� LCD ��Ļ��ʾģʽ,��ת��ʾģʽ
        {
            lcd_flag=!lcd_flag;
            if(0==lcd_flag)//������ʾ
            {
                LCD_WriteReg( R1 ,0x0000 );////��������
                LCD_WriteReg( R96,0x2700 );//��������
                LCD_Clear(Black);
            }
            else if(1==lcd_flag)//������ʾ
            {
                LCD_WriteReg( R1 ,0x0100 );////��ֱ�ԳƷ�ת����������
                LCD_WriteReg( R96,0xA700 );//ˮƽ�ԳƷ�ת����������
                LCD_Clear(Black);
            }
        }
        
        rx_pointer=0;
        memset(rxdata,0,sizeof(rxdata));
    }
}

void led_proc(void)
{
    if((uwTick-uwTick_Led)<50)return;
    uwTick_Led=uwTick;
    if(0==mode)//��Ƶģʽ��LD1����LD2��
    {
        ucled|=0x01;
        ucled&=0xfd;
    }
    else if(1==mode)//��Ƶģʽ,LD1��,LD2��
    {
        ucled|=0x02;
        ucled&=0xfe;
    }
    
    if(v_PA4>(v_PA5*y))//ָʾ�� LD3 �� 0.1 ��Ϊ����л�����״̬������Ϩ��
    {
        LD3_flag=1;
    }
    else
    {
        ucled&=0xfb;
        LD3_flag=0;
    }
    
    if(0==lcd_flag)//����,LD4��
    {
        ucled|=0x08;
    }
    else if(1==lcd_flag)//LD4��
    {
        ucled&=0xf7;
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
