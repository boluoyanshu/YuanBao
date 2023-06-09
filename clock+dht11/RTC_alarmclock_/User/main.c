/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   rtc 测试，显示时间格式为: xx:xx:xx
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火 F103-指南者 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
	
#include "stm32f10x.h"
#include "./usart/bsp_usart.h"
#include "./rtc/bsp_rtc.h"
#include "./lcd/bsp_ili9341_lcd.h"
#include "./key/bsp_key.h"  
#include "./beep/bsp_beep.h"   
#include "bsp_SysTick.h"
#include <stdio.h>
#include "./systick/bsp_SysTick.h"
#include "./dht11/bsp_dht11.h"

int xua;
int day_max;
int total_time = 0;
uint32_t clock_timestamp;
uint32_t current_timestamp;
// N = 2^32/365/24/60/60 = 136 年

/*时间结构体，默认时间2000-01-01 00:00:00*/
struct rtc_time systmtime=
{
0,0,0,1,1,2000,0
};

/*时间结构体，闹钟时间2000-01-01 00:00:08*/
struct rtc_time clocktime=
{
0,30,14,20,12,2019,0
};

extern __IO uint32_t TimeDisplay ;
extern __IO uint32_t TimeAlarm ;


//【*】注意事项：
//在bsp_rtc.h文件中：

//1.可设置宏USE_LCD_DISPLAY控制是否使用LCD显示
//2.可设置宏RTC_CLOCK_SOURCE_LSI和RTC_CLOCK_SOURCE_LSE控制使用LSE晶振还是LSI晶振

//3.STM32的LSE晶振要求非常严格，同样的电路、板子批量产品时总有些会出现问题。
//  本实验中默认使用LSI晶振。
//  
//4.！！！若希望RTC在主电源掉电后仍然运行，需要给开发板的电池槽安装钮扣电池，
//  ！！！且改成使用外部晶振模式RTC_CLOCK_SOURCE_LSE
//  钮扣电池型号：CR1220
/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main()
{		
	DHT11_Data_TypeDef DHT11_Data;
	//SysTick_Init();

	/*初始化DTT11的引脚*/
	DHT11_Init ();
	
//可使用该宏设置是否使用液晶显示
#ifdef  USE_LCD_DISPLAY
	
		ILI9341_Init ();         //LCD 初始化
		LCD_SetFont(&Font8x16);
		LCD_SetColors(WHITE,BLACK);

		ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* 清屏，显示全黑 */

		ILI9341_DispStringLine_EN(LINE(1),"        LCD alarm clock");
#endif
	
	  USART_Config();			
	
		Key_GPIO_Config();
	
		BEEP_GPIO_Config();
		
		/* 配置RTC秒中断优先级 */
	  RTC_NVIC_Config();
	  RTC_CheckAndConfig(&systmtime);
	
		/*设置闹钟寄存器*/
		clock_timestamp = mktimev(&clocktime)-TIME_ZOOM;
		RTC_SetAlarm(clock_timestamp);
		
	  while (1)
	  {
				
	    /* 每过1s 更新一次时间*/
	    if (TimeDisplay == 1)
	    {
				/* 当前时间 */
			  //TimeDisplay = 1;
				//ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);
				current_timestamp = RTC_GetCounter();
	      Time_Display( current_timestamp,&systmtime); 		  	
	      TimeDisplay = 0;
				
				/*调用DHT11_Read_TempAndHumidity读取温湿度，若成功则输出该信息*/
			if( DHT11_Read_TempAndHumidity ( & DHT11_Data ) == SUCCESS)
			{
				printf("湿度为%d.%d ％RH ，温度为 %d.%d℃ \r\n",\
				DHT11_Data.humi_int,DHT11_Data.humi_deci,DHT11_Data.temp_int,DHT11_Data.temp_deci);
			}			
			else
			{
				printf("Read DHT11 ERROR!\r\n");
			}
				
				
	    }
			
			/* 判断串口有无传入数据 */
			/* 若传入则设置闹钟 */
			if (USART_GetFlagStatus( DEBUG_USARTx ,USART_FLAG_RXNE) == SET)
			{
				clocktime.tm_year=systmtime.tm_year;
				clocktime.tm_mon=systmtime.tm_mon;
				clocktime.tm_mday=systmtime.tm_mday;
				//printf("\r\n set alarmclock time:");
				scanf("%d",&total_time);
				while(total_time<0||total_time>10000){
					printf("\n you enter erro please rewrite!");
					scanf("%d",&total_time);
				}
				printf("set %d seconds\n",total_time);
				clocktime.tm_sec = systmtime.tm_sec+(total_time%3600)%60;
				clocktime.tm_min = systmtime.tm_min+(total_time%3600)/60;
				clocktime.tm_hour = systmtime.tm_hour+(total_time/3600);
				if(clocktime.tm_sec>=60)
					{
						clocktime.tm_min+=clocktime.tm_sec/60;
						clocktime.tm_sec=clocktime.tm_sec%60;
				}
				if(clocktime.tm_min>=60)
					{
						clocktime.tm_hour+=clocktime.tm_min/60;
						clocktime.tm_min=clocktime.tm_min%60;
				}
				printf("clocktime = %d",clocktime.tm_year);
				printf("%d",clocktime.tm_mon);
				printf("%d",clocktime.tm_mday);
				printf("%d",clocktime.tm_hour);
				printf("%d",clocktime.tm_min);
				printf("%d\r\n",clocktime.tm_sec);
		    clock_timestamp = mktimev(&clocktime)-TIME_ZOOM;
		    RTC_SetAlarm(clock_timestamp);
				USART_ClearFlag(DEBUG_USARTx, USART_FLAG_RXNE);
			}
		
			
			//按下按键，通过串口修改时间
				if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON  )
			{
				struct rtc_time set_time;

				/*使用串口接收设置的时间，输入数字时注意末尾要加回车*/
				
			do 
	  {
			printf("\r\n  请输入:");
			printf("\r\n  1.set clock time");
			printf("\r\n  2.set alarm clock time ");
			scanf("%d",&xua);
			if(xua==1)
			{
				Time_Regulate_Get(&set_time);
								/*用接收到的时间设置RTC*/
				Time_Adjust(&set_time);
				
				//向备份寄存器写入标志
				BKP_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);

				break;
			}
			else if (xua==2)
			{	
				printf("\r\n=========================set alarm clock time==================");
				//printf("当前时间为: %d年 %d月 %d日 %0.2d:%0.2d:%0.2d\r\n",
	       //             systmtime.tm_year, systmtime.tm_mon, systmtime.tm_mday, 
	        //             systmtime.tm_hour, 
	        //            systmtime.tm_min, systmtime.tm_sec);

				while(1){
					clocktime.tm_year=systmtime.tm_year;
					clocktime.tm_mon=systmtime.tm_mon;
					clocktime.tm_mday=systmtime.tm_mday;
					printf("\r\n set alarmclock time: ");
					scanf("%d",&total_time);
					printf("%d seconds\n",total_time);
					if(total_time<0||total_time>10000)
						printf("\r\n you enter erro please rewrite!");
					else{
					clocktime.tm_sec = systmtime.tm_sec+(total_time%3600)%60;
					clocktime.tm_min = systmtime.tm_min+(total_time%3600)/60;
					clocktime.tm_hour = systmtime.tm_hour+(total_time/3600);
					if(clocktime.tm_sec>=60)
						{
							clocktime.tm_sec=clocktime.tm_sec%60;
							clocktime.tm_min+=clocktime.tm_sec/60;
					}
					if(clocktime.tm_min>=60)
						{
							clocktime.tm_min=clocktime.tm_min%60;
							clocktime.tm_hour+=clocktime.tm_min/60;
					}
					break;}
				}
			
				printf("%d",clocktime.tm_year);
				printf("%d",clocktime.tm_mon);
				printf("%d",clocktime.tm_mday);
				printf("%d",clocktime.tm_hour);
				printf("%d",clocktime.tm_min);
				printf("%d",clocktime.tm_sec);
		    clock_timestamp = mktimev(&clocktime)-TIME_ZOOM;
		    RTC_SetAlarm(clock_timestamp);
		
			}
		break;
	  }while(1);

			} 	
			if(clock_timestamp ==current_timestamp)
			{
			TimeAlarm =1;
			}
			//响铃
			if( TimeAlarm == 1)
			{
				BEEP(ON);
			}

			//按下按键，关闭蜂鸣器
			if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON  )
			{
				BEEP(OFF);
				TimeAlarm = 0;
			}
			//Delay_ms(1000);	
	  }

}

/***********************************END OF FILE*********************************/

