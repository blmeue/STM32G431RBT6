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
extern struct keys key[];//��main��������interrupt.c�ļ����struct keys key����
extern uint frq1,frq2;
extern float duty1,duty2;

uchar view=0;//Ĭ���ڵ�һ������

uchar pa6_duty=10;//PA6��ռ�ձ�
uchar pa7_duty=10;

extern char rxdata[30];//���մ���õ�����
extern uint8_t rxdat;//ÿ�ν��յ�����
extern uchar rx_pointer;//ȷ��һ��д���ĸ�λ����
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
    LED_Disp(0x00);//LED�ĳ�ʼ������������led��
    LCD_Init();//LCD���ĳ�ʼ��
    
    //��ʼ������
    HAL_UART_Receive_IT(&huart1,&rxdat,1);//ÿ���ж�ֻ�ܽ���һ���ַ�

	LCD_Clear(Black);//����Ļ��գ���ɺ�ɫ
	LCD_SetBackColor(Black);//���ñ���ɫ
	LCD_SetTextColor(White);//�����ı���ɫ(ǰ��ɫ)
    
	HAL_TIM_Base_Start_IT(&htim4);//������ʱ��3���ж�
    
    //PA6���Ƶ�ʹ̶�Ϊ100HZ��ռ�ձȿɵ��ڵ������ź�
    //PA7���Ƶ�ʹ̶�Ϊ200HZ��ռ�ձȿɵ��ڵ������ź�
    //��PWM���
    HAL_TIM_PWM_Start(&htim16,TIM_CHANNEL_1);//��һ������Ϊ�ĸ���ʱ�����ڶ�������Ϊͨ����
    HAL_TIM_PWM_Start(&htim17,TIM_CHANNEL_1);//��һ������Ϊ�ĸ���ʱ�����ڶ�������Ϊͨ����
    HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_1);//�򿪶�ʱ��2ͨ��1
    HAL_TIM_IC_Start_IT(&htim3,TIM_CHANNEL_1);//�򿪶�ʱ��3ͨ��1
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
//      LED_Disp(0x01);
//      HAL_Delay(500);//��ʱ500ms
//      LED_Disp(0x00);
//      HAL_Delay(500);//��ʱ500ms
      
      key_proc();
      disp_proc();
      //��ֹ���ղ�����
      if(rx_pointer!=0){
      int temp=rx_pointer;
          HAL_Delay(1);
          if(temp==rx_pointer)uart_rx_porc();
      }
      
//      char temp[20];
//      //��Ƶ�ʷ��ͳ�ȥ,���͵�����������
//      sprintf(temp,"frq=%d\n",frq1);
//      //��һ����������һ�����ڣ��ڶ��������Ƿ��͵�����
//      //������������Ҫ���͵ĳ���
//      //���ĸ������ǳ�ʱʱ��
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
      if(key[0].single_flag==1){//��һ������������
      view++;
       if(view==3)view=0;
      LCD_Clear(Black);//�л�����ʱҪ��������������������һ�������
      key[0].single_flag=0;//�����±�־λ��0�����ⱻ�ظ�ִ��  
      }
      if(key[1].single_flag==1){//�ڶ�������������
      pa6_duty+=10;
      if(pa6_duty>=100)
          pa6_duty=10;
       //�����õ�ռ�ձ�Ӧ�õ��������
      __HAL_TIM_SetCompare(&htim16,TIM_CHANNEL_1,pa6_duty);//��һ���������ĸ���ʱ�����ڶ����������ĸ�ͨ����������������ֵ
      LCD_Clear(Black);//�л�����ʱҪ��������������������һ�������
      key[1].single_flag=0;//�����±�־λ��0�����ⱻ�ظ�ִ��  
          
      }  
      if(key[2].single_flag==1){//����������������
      pa7_duty+=10;
      if(pa7_duty>=100)
          pa7_duty=10;
      __HAL_TIM_SetCompare(&htim17,TIM_CHANNEL_1,pa7_duty);//��һ���������ĸ���ʱ�����ڶ����������ĸ�ͨ����������������ֵ
      LCD_Clear(Black);//�л�����ʱҪ��������������������һ�������
      key[2].single_flag=0;//�����±�־λ��0�����ⱻ�ظ�ִ��  
          
      }  
      if(key[3].single_flag==1){//���ĸ�����������
            //��Ƶ�ʴ洢��EEPROM
          //EEPROM��8λ��Ƶ����16λ��������Ҫ��Ƶ�ʷ�Ϊ�߰�λ�͵Ͱ�λ
         uchar frq_h=frq1>>8;
          uchar frq_l=frq1&0xff;
          eeprom_write(1,frq_h);
          HAL_Delay(10);//��ʱ10����
          eeprom_write(2,frq_l);
          key[3].single_flag=0;//�����±�־λ��0�����ⱻ�ظ�ִ��  
          
      }        
}

//LCD����ʾ
void disp_proc(void){
    if(view==0){
      char text[30];//�ַ�����
      sprintf(text,"      Data       ");
      LCD_DisplayStringLine(Line1, (uint8_t *)text);//��text��������ݴ�ӡ����1��
        
      sprintf(text,"      FRQ1=%d       ",frq1);
      LCD_DisplayStringLine(Line2, (uint8_t *)text);//��text��������ݴ�ӡ����2��
      sprintf(text,"      duty1=%.3f       ",duty1);
      LCD_DisplayStringLine(Line3, (uint8_t *)text);//��text��������ݴ�ӡ����3��

     sprintf(text,"      FRQ2=%d       ",frq2);
     LCD_DisplayStringLine(Line4, (uint8_t *)text);//��text��������ݴ�ӡ����4��
     sprintf(text,"      duty2=%.3f       ",duty2);
     LCD_DisplayStringLine(Line5, (uint8_t *)text);//��text��������ݴ�ӡ����5��  
         
        sprintf(text,"      V:%.2f v      ",getADC(&hadc1));
     LCD_DisplayStringLine(Line6, (uint8_t *)text);//��text��������ݴ�ӡ����6��  
     
        sprintf(text,"      V:%.2f v     ",getADC(&hadc2));
     LCD_DisplayStringLine(Line7, (uint8_t *)text);//��text��������ݴ�ӡ����7��  
            
     uint eep_temp=(eeprom_read(1)<<8)+(eeprom_read(2));//�Զ����������ݽ��д���
     
     sprintf(text,"      FRQ_eep=%d     ",eep_temp);
     LCD_DisplayStringLine(Line8, (uint8_t *)text);//��text��������ݴ�ӡ����7��  
        
    }
    if(view==1){
      char text[30];//�ַ�����
      sprintf(text,"      Para       ");
      LCD_DisplayStringLine(Line1, (uint8_t *)text);//��text��������ݴ�ӡ���ھ���
        sprintf(text,"    PA6:%d       ",pa6_duty);
        LCD_DisplayStringLine(Line2, (uint8_t *)text);//��text��������ݴ�ӡ���ھ���
        sprintf(text,"    PA7:%d       ",pa7_duty);
        LCD_DisplayStringLine(Line4, (uint8_t *)text);//��text��������ݴ�ӡ���ھ���
 
    }
    if(view==2){
      char text[30];//�ַ�����
      sprintf(text,"      Car_msg       ");
        
      LCD_DisplayStringLine(Line1, (uint8_t *)text);//��text��������ݴ�ӡ���ھ���
        sprintf(text,"car_type=%s",car_type);
      LCD_DisplayStringLine(Line2, (uint8_t *)text);//��text��������ݴ�ӡ���ھ���
        sprintf(text,"car_data=%s",car_data);
      LCD_DisplayStringLine(Line3, (uint8_t *)text);//��text��������ݴ�ӡ���ھ���
        sprintf(text,"car_tim=%s",car_time);
      LCD_DisplayStringLine(Line4, (uint8_t *)text);//��text��������ݴ�ӡ���ھ���
        
    }
}

//���պ�������λ������
//���գ��ӵ����ϵĴ�����������Ҫ���͵��ַ�����Ƭ��ͨ�����ڽ��յ�
//���ͣ���Ƭ��ͨ�����ڷ������ݵ������ϣ����ڴ��������Ͽ��Կ������͵�������Ϣ
void uart_rx_porc()
{
    //�ж��Ƿ��н��յ�����
    if(rx_pointer>0)//���յ���
    {
        if(rx_pointer==22)//�Ƿ���յ���22���ַ�
        {
            //��rxdata���
            sscanf(rxdata,"%4s:%4s:%12s",car_type,car_data,car_time);
            
        }
        else//û�н��յ�22���ַ�������Error
        {
      char temp[20];
      //��Ƶ�ʷ��ͳ�ȥ,���͵�����������
      sprintf(temp,"Error");
      //��һ����������һ�����ڣ��ڶ��������Ƿ��͵�����
      //������������Ҫ���͵ĳ���
      //���ĸ������ǳ�ʱʱ��
      HAL_UART_Transmit(&huart1,(uint8_t *)temp,strlen(temp),50);           
        }
       rx_pointer=0;
       memset(rxdata,0,30);//��rxdata���
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
