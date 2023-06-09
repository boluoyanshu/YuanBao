/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   rtc ���ԣ���ʾʱ���ʽΪ: xx:xx:xx
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:���� F103-ָ���� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
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
// N = 2^32/365/24/60/60 = 136 ��

/*ʱ��ṹ�壬Ĭ��ʱ��2000-01-01 00:00:00*/
struct rtc_time systmtime=
{
0,0,0,1,1,2000,0
};

/*ʱ��ṹ�壬����ʱ��2000-01-01 00:00:08*/
struct rtc_time clocktime=
{
0,30,14,20,12,2019,0
};

extern __IO uint32_t TimeDisplay ;
extern __IO uint32_t TimeAlarm ;


//��*��ע�����
//��bsp_rtc.h�ļ��У�

//1.�����ú�USE_LCD_DISPLAY�����Ƿ�ʹ��LCD��ʾ
//2.�����ú�RTC_CLOCK_SOURCE_LSI��RTC_CLOCK_SOURCE_LSE����ʹ��LSE������LSI����

//3.STM32��LSE����Ҫ��ǳ��ϸ�ͬ���ĵ�·������������Ʒʱ����Щ��������⡣
//  ��ʵ����Ĭ��ʹ��LSI����
//  
//4.��������ϣ��RTC������Դ�������Ȼ���У���Ҫ��������ĵ�ز۰�װť�۵�أ�
//  �������Ҹĳ�ʹ���ⲿ����ģʽRTC_CLOCK_SOURCE_LSE
//  ť�۵���ͺţ�CR1220
/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main()
{		
	DHT11_Data_TypeDef DHT11_Data;
	//SysTick_Init();

	/*��ʼ��DTT11������*/
	DHT11_Init ();
	
//��ʹ�øú������Ƿ�ʹ��Һ����ʾ
#ifdef  USE_LCD_DISPLAY
	
		ILI9341_Init ();         //LCD ��ʼ��
		LCD_SetFont(&Font8x16);
		LCD_SetColors(WHITE,BLACK);

		ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* ��������ʾȫ�� */

		ILI9341_DispStringLine_EN(LINE(1),"        LCD alarm clock");
#endif
	
	  USART_Config();			
	
		Key_GPIO_Config();
	
		BEEP_GPIO_Config();
		
		/* ����RTC���ж����ȼ� */
	  RTC_NVIC_Config();
	  RTC_CheckAndConfig(&systmtime);
	
		/*�������ӼĴ���*/
		clock_timestamp = mktimev(&clocktime)-TIME_ZOOM;
		RTC_SetAlarm(clock_timestamp);
		
	  while (1)
	  {
				
	    /* ÿ��1s ����һ��ʱ��*/
	    if (TimeDisplay == 1)
	    {
				/* ��ǰʱ�� */
			  //TimeDisplay = 1;
				//ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);
				current_timestamp = RTC_GetCounter();
	      Time_Display( current_timestamp,&systmtime); 		  	
	      TimeDisplay = 0;
				
				/*����DHT11_Read_TempAndHumidity��ȡ��ʪ�ȣ����ɹ����������Ϣ*/
			if( DHT11_Read_TempAndHumidity ( & DHT11_Data ) == SUCCESS)
			{
				printf("ʪ��Ϊ%d.%d ��RH ���¶�Ϊ %d.%d�� \r\n",\
				DHT11_Data.humi_int,DHT11_Data.humi_deci,DHT11_Data.temp_int,DHT11_Data.temp_deci);
			}			
			else
			{
				printf("Read DHT11 ERROR!\r\n");
			}
				
				
	    }
			
			/* �жϴ������޴������� */
			/* ���������������� */
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
		
			
			//���°�����ͨ�������޸�ʱ��
				if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON  )
			{
				struct rtc_time set_time;

				/*ʹ�ô��ڽ������õ�ʱ�䣬��������ʱע��ĩβҪ�ӻس�*/
				
			do 
	  {
			printf("\r\n  ������:");
			printf("\r\n  1.set clock time");
			printf("\r\n  2.set alarm clock time ");
			scanf("%d",&xua);
			if(xua==1)
			{
				Time_Regulate_Get(&set_time);
								/*�ý��յ���ʱ������RTC*/
				Time_Adjust(&set_time);
				
				//�򱸷ݼĴ���д���־
				BKP_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);

				break;
			}
			else if (xua==2)
			{	
				printf("\r\n=========================set alarm clock time==================");
				//printf("��ǰʱ��Ϊ: %d�� %d�� %d�� %0.2d:%0.2d:%0.2d\r\n",
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
			//����
			if( TimeAlarm == 1)
			{
				BEEP(ON);
			}

			//���°������رշ�����
			if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON  )
			{
				BEEP(OFF);
				TimeAlarm = 0;
			}
			//Delay_ms(1000);	
	  }

}

/***********************************END OF FILE*********************************/

