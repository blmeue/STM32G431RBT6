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

#include "lcd.h"
#include "led.h"
#include "myusart.h"
#include "timer.h"
#include "myadc.h"

#include "stdio.h"
#include "string.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

__IO uint32_t uwTick_Lcd;
__IO uint32_t uwTick_Led;
__IO uint32_t uwTick_Usart;
unsigned char ucled=0x00;
unsigned char lcd_text[30];
unsigned char usart_text[30];

unsigned char view=0;
unsigned char N=0;//��������:0:ֱ�У�1:��ת��2����ת
double D;//��ǰ�������ľ��룬��λΪ��
double R37_V;

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

void Lcd_proc(void);
void key_proc(void);
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
  MX_TIM4_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
    
    led_disp(ucled);
    
    LCD_Init();
    LCD_Clear(Black);
    LCD_SetBackColor(Black);
    LCD_SetTextColor(White);
    
    HAL_TIM_Base_Start_IT(&htim4);
    
    HAL_UART_Receive_IT(&huart1,&rxdat,1);
    
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
          if(temp==rx_pointer)
          {
              usart_proc();
          }
      }   
      
      Lcd_proc();

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

void Lcd_proc(void)
{
    if((uwTick-uwTick_Lcd)<100)return;
    uwTick_Lcd=uwTick;
    
     R37_V=Get_Adc(&hadc2);
    
    if(R37_V>=3.0)
    {
        D=300;
    }
    else
    {
        D=R37_V*100.0;
    }
    switch(view)
    {
        case 0:
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"        DATA          ");
            LCD_DisplayStringLine(Line1,lcd_text);
        
            memset(lcd_text,0,sizeof(lcd_text));
            switch(N)
            {
                case 0://ֱ��
                    sprintf((char *)lcd_text,"       N:S       ");
                    break;
                case 1://��ת
                    sprintf((char *)lcd_text,"       N:R       ");
                    break;
                case 2://��ת
                    sprintf((char *)lcd_text,"       N:L       ");
                    break;
            }
            LCD_DisplayStringLine(Line3,lcd_text);
        
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"       D:%0.1f       ",D);
            LCD_DisplayStringLine(Line4,lcd_text);
            
            break;
        case 1://ƫ�뷽�����
            memset(lcd_text,0,sizeof(lcd_text));
            sprintf((char *)lcd_text,"        WARN          ");
            LCD_DisplayStringLine(Line4,lcd_text);
            
            break;
    }
}

void key_proc(void)
{

    
    if(1==key[0].single_flag)//��ƫ�뵼�������£�����B1��ʾ������ͷ�ɹ��������л������ݽ��棬ͬʱ���������Ϊֱ��
    {
        key[0].single_flag=0;
        if(1==view)
        {
            //��ͷ�ɹ�
            view=0;
            N=0;
            LCD_Clear(Black);
            //������ͷ�ɹ������ع̶��ַ�����Success\r\n����
            memset(usart_text,0,sizeof(usart_text));
            sprintf((char *)usart_text,"Success\r\n");
            HAL_UART_Transmit(&huart1,usart_text,9,50);
        }
    }
    else if(1==key[1].single_flag)
    {
        key[1].single_flag=0;
    }
    else if(1==key[2].single_flag)//�ڳ�����ʻ���ݽ����£�����������Ϊ��תʱ��5S�ڰ���B3�����ת��5S��δ����B3�����л�δƫ�뵼������
    {
        key[2].single_flag=0;
        if(0==view)
        {
            //�����ת�󣬵��������л���ֱ��
            if(2==N&&(L_sec<500))
            {
                N=0;
                //LCD_Clear(Black);
                //L_sec=0;
                memset(usart_text,0,sizeof(usart_text));
                sprintf((char *)usart_text,"Success\r\n");
                HAL_UART_Transmit(&huart1,usart_text,9,20);
            }
            else if(0==N)//�ڵ�������Ϊֱ�У�S��ʱ,������δ���յ��ַ���L����R��������B3��B4�������л�Ϊƫ�뵼�����档
            {
                view=1;
                LCD_Clear(Black);
                //����δ���յ��ַ���L����R��,���������ת����תʱ�����ڷ����ַ�����Warn\r\n����
                memset(usart_text,0,sizeof(usart_text));
                sprintf((char *)usart_text,"Warn\r\n");
                HAL_UART_Transmit(&huart1,usart_text,6,20);
            }
            else if(L_sec>=500)//����5S����ת���л�Ϊƫ�뵼������
            {
                view=1;
                //L_sec=0;
                LCD_Clear(Black);
            }
            L_sec=0;
        }

    }
    else if(1==key[3].single_flag)//�ڳ�����ʻ���ݽ����£�����������Ϊ��תʱ��5S�ڰ���B3�����ת��5S��δ����B3�����л�δƫ�뵼������
    {
        key[3].single_flag=0;
        if(0==view)
        {
            
            //�����ת�󣬵��������л���ֱ��
            if(1==N&&(R_sec<500))
            {
                N=0;
                //LCD_Clear(Black);
                memset(usart_text,0,sizeof(usart_text));
                sprintf((char *)usart_text,"Success\r\n");
                HAL_UART_Transmit(&huart1,usart_text,9,20);
            }
            else if(0==N)//�ڵ�������Ϊֱ�У�S��ʱ��������δ���յ��ַ���L����R��������B3��B4�������л�Ϊƫ�뵼�����档
            {
                view=1;
                LCD_Clear(Black);
                //����δ���յ��ַ���L����R��,���������ת����תʱ�����ڷ����ַ�����Warn\r\n����
                memset(usart_text,0,sizeof(usart_text));
                sprintf((char *)usart_text,"Warn\r\n");
                HAL_UART_Transmit(&huart1,usart_text,6,20);
            }  
            else if(R_sec>=500)//����5S����ת���л�Ϊƫ�뵼������
            {
                view=1;
                //R_sec=0;
                LCD_Clear(Black);
            }
            R_sec=0;

        }
        
    }
    
    if(0==view)
    {
       if((1==N)&&(R_sec>500))//��ת,��5S��δ����B3��
       {
           view=1;
           N=0;
           R_sec=0;
           LCD_Clear(Black);
           memset(usart_text,0,sizeof(usart_text));
           sprintf((char *)usart_text,"Warn\r\n");
           HAL_UART_Transmit(&huart1,usart_text,7,20);
       }  
       else if(2==N&&(L_sec>500))//��ת,��5S��δ����B3��
        {
            view=1;
            N=0;
            L_sec=0;
            LCD_Clear(Black);
            memset(usart_text,0,sizeof(usart_text));
            sprintf((char *)usart_text,"Warn\r\n");
            HAL_UART_Transmit(&huart1,usart_text,7,20);
        }
    }
}

void usart_proc(void)
{
    if((uwTick-uwTick_Usart)<20)return;
    uwTick_Usart=uwTick;
    if(rx_pointer!=0)
    {
        //������ʻ���ݽ�����
        if(0==view){
            if(0==strcmp((char *)rxdata,"L"))
            {
                //�����յ��ַ��󣬵��������Ϊ��ת
                N=2;
               
            }
            else if(0==strcmp((char *)rxdata,"R"))
            {
                //�����յ��ַ�R�����������Ϊ��ת
                N=1;
                
            }
            else
            {
               //�����յ������Ƿ��ַ�ʱ�����ع̶��ַ�����ERROR\r\n����
               memset(usart_text,0,sizeof(usart_text));
               sprintf((char *)usart_text,"ERROR\r\n");
               HAL_UART_Transmit(&huart1,usart_text,7,20);
            }
        }
        //ƫ�뵼��������
        else if(1==view)
        {
            //�����յ������ַ������ع̶��ַ�����WAIT\r\n����
            memset(usart_text,0,sizeof(usart_text));
            sprintf((char *)usart_text,"WAIT\r\n");
            HAL_UART_Transmit(&huart1,usart_text,6,20);    
        }
        rx_pointer=0;
        memset(rxdata,0,sizeof(rxdata));
    }
}

void led_proc(void)
{
    if((uwTick-uwTick_Led)<50)return;
    uwTick_Led=uwTick;
    //��ƫ�뵼�������£�ָʾ��LD8����������Ϩ��
    if(1==view)
    {
        ucled|=0x80;
    }
    else
    {
        ucled&=0x7f;
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
