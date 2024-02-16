
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
//#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "led/bsp_led.h" //bsp文件里的led文件夹里的bsp_led.h文件
#include "key\key.h"
#include "lcd\lcd.h"
#include "stdio.h"
#include "bspusart/bspusart.h"
#include "String.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
//***全局变量声明区
//**减速变量
__IO uint32_t uwTick_Led = 0;//控制Led_Proc的执行速度
__IO uint32_t uwTick_Key = 0;//控制Key_Proc的执行速度
__IO uint32_t uwTick_Lcd =0;//控制Lcd_Pric 的执行速度
 __IO uint32_t uwTick_Usart =0;//Usart_Proc 的执行速度
 
//*LED专属变量
uint8_t ucLed =0;
 
//*按键专属变量
uint8_t ucKey_Val,ucKey_Down,ucKey_Up,ucKey_Old;

unsigned char i;
//lcd显示专用变量
unsigned char lcd_Disp_String[22];

//usart专用变量
int counter=0;
char str[40];
char car_type[5];
char car_data[5];
char car_time[13];

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

//***子函数声明区
 
void Led_Proc(void);
void Key_Proc(void);
void Lcd_Proc(void);
void Usart_Proc(void);

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
    //MX_GPIO_Init();
       key_Init();
   LED_LCD_Init();

    
   HAL_UART_Receive_IT(&huart1,&rxdat,1);//每次中断只能接收一个字符

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();
  USART1_Init();
  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  //MX_GPIO_Init();
  /* USER CODE BEGIN 2 */
    LED_Disp(0X00);
    LCD_Init();
	LCD_Clear(White);//清屏为白色，不清屏默认为黑色
	LCD_SetBackColor(White);//设置背景颜色，某一行的背景颜色，不设置的话就默认为白色
	LCD_SetTextColor(Blue);//设置字体颜色，某一行的字的颜色，不设置的话就默认为黑色
	

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      Key_Proc();
      Led_Proc();
      Lcd_Proc();
     // Usart_Proc();
      //防止接收不完整
      if(rx_pointer!=0){
      int temp=rx_pointer;
          HAL_Delay(1);
          if(temp==rx_pointer)Usart_Proc();
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

//串口发送
void Usart_Proc(void)
{
// 	if((uwTick - uwTick_Usart) < 500) return; //减速函数
//		uwTick_Usart = uwTick;   
    //单片机串口往外发消息,在电脑的串口助手上可以看到发送的数据
//    sprintf(str,"%04d:Hello,world.\r\n",counter);
//    HAL_UART_Transmit(&huart1,(unsigned char*)str,strlen(str),50);
//    HAL_Delay(500);
//    if(++counter==10000){
//        counter=0;
//    }
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


/* USER CODE BEGIN 4 */

//***led扫描子函数
 
void Led_Proc(void)
{
	if((uwTick - uwTick_Led) < 200) return; //减速函数
		uwTick_Led = uwTick;
	
	LED_Disp(ucLed);
}
 
 
//***按键扫描子函数
 
void Key_Proc(void)
{
 //uwTick（全局变量来的）这个值是系统定时间自带的一个计数的,它是一个向上计数的，在系统运行的时候，不断加一，用来当延时函数非常的方便！！！
	if((uwTick - uwTick_Key) < 50) return; //减速函数//50毫秒一次
		uwTick_Key = uwTick;
	
	ucKey_Val = Key_Scan();
	ucKey_Down = ucKey_Val & (ucKey_Old ^ ucKey_Val);//ucKey_Old ^ ucKey_Val：相同为0，不同为1
	ucKey_Up = ~ucKey_Val & (ucKey_Old ^ ucKey_Val);
	ucKey_Old = ucKey_Val;
	
	if(ucKey_Down == 4)
	{
		 ucLed = 0x88;
	}
	if(ucKey_Down == 3)
	{
		 ucLed = 0x00;
	}
    if(ucKey_Down == 2){
        ucLed = 0x55;
    }
    if(ucKey_Down == 1){
        ucLed = 0x44;
    }    
	
}

//Lcd
void Lcd_Proc(void)
{
	if((uwTick - uwTick_Lcd) < 300) return; //减速函数//300毫秒刷新一次
		uwTick_Lcd = uwTick;
    i++;
    sprintf((char *)lcd_Disp_String,"  i num:%03d           ",(unsigned int)i);
    LCD_DisplayStringLine(Line2, (uint8_t *)lcd_Disp_String);
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
