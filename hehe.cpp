#include "sys.h"
#include "delay.h"  
#include "led.h"
#include "usmart.h"		
#include "hc05.h" 	 
#include "usart3.h" 	
#include "key.h" 	 
#include "string.h"	 
#include "pwm.h"
#include "usart.h"
#include "syn6288.h"
#include "dht11.h"
#include "lcd.h"
#include "beep.h"
#include "fire.h"
#include "adc3.h"
#include "lsens.h"
#include "timer.h "
#include "esp8266.h"
/*À¶ÑÀ(USART3)£ºTXD-PB11;
        RXD-PB10;
¶æ»ú PF9;
µç»ú PA5,PA6£»
ÓïÒô²¥±¨½ÓÏß¶¨Òå(USART6)£º
		VCC--3.3V
		RXD--PG14
		TXD--PG9
		GND--GND
ÓïÒôÊ¶±ð½ÓÏß¶¨Òå(USART2)£º
    VCC--3.3V
		RXD--PA2
		TXD--PA3
		GND--GND	
¼ÌµçÆ÷£ºF5,F10,F6,F7£»
·äÃùÆ÷£ºF8£»
»ðÑæ´«¸ÐÆ÷£ºE5£»
¹âÃô£ºF7;
*/


//Ñ¡Ôñ±³¾°ÒôÀÖ2¡£(£ºÎÞ±³¾°ÒôÀÖ  1-15£º±³¾°ÒôÀÖ¿ÉÑ¡)
		//m[0~16]:0±³¾°ÒôÀÖÎª¾²Òô£¬16±³¾°ÒôÀÖÒôÁ¿×î´ó
		//v[0~16]:0ÀÊ¶ÁÒôÁ¿Îª¾²Òô£¬16ÀÊ¶ÁÒôÁ¿×î´ó
		//t[0~5]:0ÀÊ¶ÁÓïËÙ×îÂý£¬5ÀÊ¶ÁÓïËÙ×î¿ì
		//ÆäËû²»³£ÓÃ¹¦ÄÜÇë²Î¿¼Êý¾ÝÊÖ²á¡¢
		
//ALIENTEK Ì½Ë÷ÕßSTM32F407¿ª·¢°å À©Õ¹ÊµÑé1
//ATK-HC05À¶ÑÀ´®¿ÚÄ£¿éÊµÑé -¿âº¯Êý°æ±¾
//¼¼ÊõÖ§³Ö£ºwww.openedv.com
//ÌÔ±¦µêÆÌ£ºhttp://eboard.taobao.com  
//¹ãÖÝÊÐÐÇÒíµç×Ó¿Æ¼¼ÓÐÏÞ¹«Ë¾  
//×÷Õß£ºÕýµãÔ­×Ó @ALIENTEK

/**************Ð¾Æ¬ÉèÖÃÃüÁî*********************/
u8 SYN_StopCom[] = {0xFD, 0X00, 0X02, 0X02, 0XFD}; //Í£Ö¹ºÏ³É
u8 SYN_SuspendCom[] = {0XFD, 0X00, 0X02, 0X03, 0XFC}; //ÔÝÍ£ºÏ³É
u8 SYN_RecoverCom[] = {0XFD, 0X00, 0X02, 0X04, 0XFB}; //»Ö¸´ºÏ³É
u8 SYN_ChackCom[] = {0XFD, 0X00, 0X02, 0X21, 0XDE}; //×´Ì¬²éÑ¯
u8 SYN_PowerDownCom[] = {0XFD, 0X00, 0X02, 0X88, 0X77}; //½øÈëPOWER DOWN ×´Ì¬ÃüÁî


int main(void)
{ 
	u16 len;
	u16 key=0;
	u16 mode=0;//¶æ»úÄ£Ê½ 1.ÊÖ¶¯Ä£Ê½ 2.×Ô¶¯Ä£Ê½ 3.Ëø¶¨Ä£Ê½
	u8 adc;
	u16 adcx;
	float temp;
	short t=9999;
	u8 a=0;
  u8 temperature;  	    
	u8 humidity;	
	u8 m =0;
	u8 h =0;
	u8 o =0;
//	u8 adcx;
	u8 reclen=0;  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//ÉèÖÃÏµÍ³ÖÐ¶ÏÓÅÏÈ¼¶·Ö×é2
	delay_init(168);      //³õÊ¼»¯ÑÓÊ±º¯Êý
	usart3_init(115200);    //³õÊ¼»¯´®¿Ú3£¨À¶ÑÀ£©
	usmart_dev.init(84); 		//³õÊ¼»¯USMART	
  usart2_init(9600);      //³õÊ¼»¯´®¿Ú2£¨ÓïÒôÊ¶±ð£©
	uart6_init(9600);       //³õÊ¼»¯´®¿Ú6£¨ÓïÒô²¥±¨£©
  uart_init(115200);	    //³õÊ¼»¯´®¿Ú1£¨LCD£©
	Adc3_Init();        //³õÊ¼»¯ADC
	Adc_Init(); 
	LED_Init();					//³õÊ¼»¯LED
	BEEP_Init();        //³õÊ¼»¯·äÃùÆ÷
	KEY_Init();					//³õÊ¼»¯°´¼ü 
	fire_init();        //³õÊ¼»¯»ðÑæ´«¸ÐÆ÷
	Lsens_Init(); 			//³õÊ¼»¯¹âÃô´«¸ÐÆ÷
	LCD_Init();         //³õÊ¼»¯LCD
//	esp8266_start_trans();
  TIM7_Int_Init(100-1,8400-1);
//	TIM14_PWM_Init(200-1,8400-1);
//  TIM13_PWM_Init(200-1,8400-1);

	TIM13_PWM_Init(20000-1,42-1);	//42M/42=1MhzµÄ¼ÆÊýÆµÂÊ,ÖØ×°ÔØÖµ20000£¬ËùÒÔPWMÆµÂÊÎª 1M/20000=50hz.  
  TIM14_PWM_Init(20000-1,42-1);
	
	USART3_RX_STA=0;
	POINT_COLOR=BLUE;//ÉèÖÃ×ÖÌåÎªÀ¶É«
	LCD_ShowString(30,130,200,16,16,"LSENS_VAL:");	             	
 	LCD_ShowString(30,150,200,16,16,"Temp:  C");	 
 	LCD_ShowString(30,170,200,16,16,"Humi:  %");
	LCD_ShowString(30,190,200,16,16,"ADC1_CH5_VAL:");	      
	LCD_ShowString(30,210,200,16,16,"ADC1_CH5_VOL:0.000V");	//ÏÈÔÚ¹Ì¶¨Î»ÖÃÏÔÊ¾Ð¡Êýµã 
	esp8266_start_trans();
 	while(1) 
	{		 
				
		/*¹âÃô*/
		adc=Lsens_Get_Val();
		LCD_ShowxNum(30+10*8,130,adc,3,16,0);//ÏÔÊ¾ADCµÄÖµ 
		delay_ms(10);
		if(adc<30)
		{
			LED3=0;   //´ò¿ª¿ÍÌüµÆ
			delay_ms(150);
		}
		
    /*DHT11*/
		if(a%10==0)//Ã¿100ms¶ÁÈ¡Ò»´Î
		{									  
			DHT11_Read_Data(&temperature,&humidity);		//¶ÁÈ¡ÎÂÊª¶ÈÖµ					    
			LCD_ShowNum(30+40,150,temperature,2,16);		//ÏÔÊ¾ÎÂ¶È	   		   
			LCD_ShowNum(30+40,170,humidity,2,16);			//ÏÔÊ¾Êª¶È	 	   
		}				   
	 	delay_ms(10);
		a++;
		if(a==20)
		{
			a=0;
			LED0=!LED0;
		}
		
		adcx=Get_Adc_Average(ADC_Channel_4,20);//»ñÈ¡Í¨µÀ5µÄ×ª»»Öµ£¬20´ÎÈ¡Æ½¾ù
		LCD_ShowxNum(134,190,adcx,4,16,0);    //ÏÔÊ¾ADCC²ÉÑùºóµÄÔ­Ê¼Öµ
		temp=(float)adcx*(3.3/4096);          //»ñÈ¡¼ÆËãºóµÄ´øÐ¡ÊýµÄÊµ¼ÊµçÑ¹Öµ£¬±ÈÈç3.1111
		adcx=temp;                            //¸³ÖµÕûÊý²¿·Ö¸øadcx±äÁ¿£¬ÒòÎªadcxÎªu16ÕûÐÎ
		LCD_ShowxNum(134,210,adcx,1,16,0);    //ÏÔÊ¾µçÑ¹ÖµµÄÕûÊý²¿·Ö£¬3.1111µÄ»°£¬ÕâÀï¾ÍÊÇÏÔÊ¾3
		temp-=adcx;                           //°ÑÒÑ¾­ÏÔÊ¾µÄÕûÊý²¿·ÖÈ¥µô£¬ÁôÏÂÐ¡Êý²¿·Ö£¬±ÈÈç3.1111-3=0.1111
		temp*=1000;                           //Ð¡Êý²¿·Ö³ËÒÔ1000£¬ÀýÈç£º0.1111¾Í×ª»»Îª111.1£¬Ïàµ±ÓÚ±£ÁôÈýÎ»Ð¡Êý¡£
		LCD_ShowxNum(150,210,temp,3,16,0X80); //ÏÔÊ¾Ð¡Êý²¿·Ö£¨Ç°Ãæ×ª»»ÎªÁËÕûÐÎÏÔÊ¾£©£¬ÕâÀïÏÔÊ¾µÄ¾ÍÊÇ111.
		LED0=!LED0;
		delay_ms(250);	
		
		
		/*»ðÑæ´«¸ÐÆ÷*/
//		if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_5))		//GPIOE5Òý½ÅÊÇ·ñ¸ßµÍµçÆ½
//	  {
////			printf("Ã»ÓÐ»ðÔ´\r\n");		  //printf(ÎÞ»ð)
//		  //ÈÃ·äÃùÆ÷²»Ïì
//			GPIO_ResetBits(GPIOF,GPIO_Pin_8);
//	  }
//	  else
//	  {
////			printf("ÓÐ»ð\r\n");		//printf()ÓÐ»ð
//		  //ÈÃ·äÃùÆ÷Ïì
//			GPIO_SetBits(GPIOF,GPIO_Pin_8);

//	  }

//esp8266

     if(m==10)
		 {
			 m=0;
			 
			 //½«ÎÂ¶ÈºÍÊª¶ÈÉÏ´«µ½ÔÆÆ½Ì¨
			 numToString(temperature);
//			 printf("temperature: %s,",strValue);
			 //LCD_ShowString(30,70,200,16,16,(u8 *)strValue);
			 esp8266_str_data("temp",strValue);
			 numToString(humidity);
//			 printf("humidity: %s\r\n\r\n",strValue);
			 //LCD_ShowString(30,90,200,16,16,(u8 *)strValue);
			 esp8266_str_data("humi",strValue);
			 numToString(adc);
			 esp8266_str_data("sun",strValue);
//			 LED_shan();
		 }
			delay_ms(10);
			m++;
	   h=esp8266_get_data("qwe");
		 if(h==1)
		 {
		    TIM_SetCompare1(TIM14,1200);
	      TIM_SetCompare1(TIM13,1900);
		 }
		 else
		 {
		    TIM_SetCompare1(TIM14,1500);
	      TIM_SetCompare1(TIM13,1500);
		 }
	
//		
//		
////		/*À¶ÑÀ*/
////		if(USART3_RX_STA&0X8000)			//½ÓÊÕµ½Ò»´ÎÊý¾ÝÁË
////		{
//// 			reclen=USART3_RX_STA&0X7FFF;	//µÃµ½Êý¾Ý³¤¶È
////		  	USART3_RX_BUF[reclen]=0;	 	//¼ÓÈë½áÊø·û
////			if(reclen) 		//¿ØÖÆDS1¼ì²â
////			{
////				if(strcmp((const char*)USART3_RX_BUF,"+LED1 ON")==0)
////				{
////			      LED1=0;	//´ò¿ªÎÔÊÒµÆ
////					 SYN_FrameInfo(2, "[v9][m1][t5]Ö÷ÈËÎÒÔÚ");
////					 printf("0");
////				}
////				if(strcmp((const char*)USART3_RX_BUF,"+LED1 OFF")==0)
////				{
////					  LED1=1;//¹Ø±ÕÎÔÊÒµÆ
////					 SYN_FrameInfo(2, "[v9][m1][t5]Ö÷ÈËÎÒÀë¿ªÁË");
////					  printf("0");
////				}	
////				if(strcmp((const char*)USART3_RX_BUF,"+PWM ON")==0)
////				{
////				    TIM_SetCompare1(TIM14,1500);
////		        TIM_SetCompare1(TIM13,1500);
////					  printf("OK");
////				}
////				if(strcmp((const char*)USART3_RX_BUF,"+PWM OFF")==0)//¹Ø±ÕLED1
////				
////				{
////				   TIM_SetCompare1(TIM14,1100);
////				   TIM_SetCompare1(TIM13,2000);
////				}
//				
//		
//				
////				if(strcmp((const char*)USART3_RX_BUF,"+LED0 ON")==0)LED0=0;	//´ò¿ª³ø·¿µÆ
////				if(strcmp((const char*)USART3_RX_BUF,"+LED0 OFF")==0)LED0=1;//¹Ø±Õ³ø·¿µÆ
////				if(strcmp((const char*)USART3_RX_BUF,"+LED2 ON")==0)LED2=0;	//´ò¿ªÎÀÉú¼äµÆ
////				if(strcmp((const char*)USART3_RX_BUF,"+LED2 OFF")==0)LED2=1;//¹Ø±ÕÎÀÉú¼äµÆ
////				if(strcmp((const char*)USART3_RX_BUF,"+LED3 ON")==0)LED3=0;	//´ò¿ª¿ÍÌüµÆ
////				if(strcmp((const char*)USART3_RX_BUF,"+LED3 OFF")==0)LED3=1;//¹Ø±Õ¿ÍÌüµÆ
////				if(strcmp((const char*)USART3_RX_BUF,"+LED4 ON")==0)TIM_SetCompare1(TIM14,15);	//¿ªÃÅ
////				if(strcmp((const char*)USART3_RX_BUF,"+LED4 OFF")==0)TIM_SetCompare1(TIM14,5);	//¹ØÃÅ
////				if(strcmp((const char*)USART3_RX_BUF,"+LED5 ON")==0)//¿ª´°
////				{
////					MOTOR_R=0;
////					MOTOR_L=1;
////				  delay_ms(1000);
////					MOTOR_R=0;
////					MOTOR_L=0;
////				}
////				if(strcmp((const char*)USART3_RX_BUF,"+LED5 OFF")==0) //¹Ø´°
////				{
////					MOTOR_R=1;
////					MOTOR_L=0;
////				  delay_ms(1000);
////					MOTOR_R=0;
////					MOTOR_L=0;
////				}
////			}
//// 			USART3_RX_STA=0;	 
////		}	 															     				   
////		t++;	
//		
//		
//		
////	 key = KEY_Scan(0);		
////	   	if(key)
////		{						   
////			switch(key)			
////			{				 
////				case WKUP_PRES:	  //0

////					TIM_SetCompare1(TIM14,1500);
////		      TIM_SetCompare1(TIM13,1500);
////			    break;
////				case KEY0_PRES:	//-90
////					
////				  TIM_SetCompare1(TIM14,1200);
////				  TIM_SetCompare1(TIM13,1900);
////				  break;
////			  case KEY1_PRES:	//90 

////				  TIM_SetCompare1(TIM14,1300);
////				  TIM_SetCompare1(TIM13,1750);
////				  break;
////			}
////			
////		}	

   key=KEY_Scan(0);
		if(key==4)
		{
		    mode++;
			  if(mode>2)
				mode=0;
		}
	if(mode==1)//ÊÖ¶¯Ä£Ê½
	{
		  if(adc<50)
    {
      TIM_SetCompare1(TIM14,1500);
	    TIM_SetCompare1(TIM13,1500);
	    GPIO_SetBits(GPIOA,GPIO_Pin_5);
    }
     if(adc>50)
   { 
     TIM_SetCompare1(TIM14,1200);
	   TIM_SetCompare1(TIM13,1900);
	  GPIO_ResetBits(GPIOA,GPIO_Pin_5);
   }
	}
	if(mode==2)
	 {
		   if(adcx<2)
      {
	        TIM_SetCompare1(TIM14,1500);
	        TIM_SetCompare1(TIM13,1500);
      }
       if(adcx>2)
      {
          TIM_SetCompare1(TIM14,1200);
	        TIM_SetCompare1(TIM13,1900);
      }
		
	 }
	 
  
	 
	 

	if(humidity>20)
 {
	 GPIO_SetBits(GPIOA,GPIO_Pin_6);
 }
  if(humidity<20)
 {
	 GPIO_ResetBits(GPIOA,GPIO_Pin_6);
 }
		


 
		/*ÓïÒô*/
		if(USART2_RX_STA&0x8000)
		{		
      			
			len=USART2_RX_STA&0x3fff;//µÃµ½´Ë´Î½ÓÊÕµ½µÄÊý¾Ý³¤¶È
			for(t=0;t<len;t++)
			{
				if(USART2_RX_BUF[0]=='0')//½ÓÊÕµ½51µ¥Æ¬»ú·¢¹ýÀ´µÄÐÅºÅ0
				{
					SYN_FrameInfo(2, "[v9][m1][t5]Ö÷ÈËÎÒÔÚ");
					printf("OKKKK");
					delay_ms(50);
					
				}
				if(USART2_RX_BUF[0]=='1')//½ÓÊÕµ½51µ¥Æ¬»ú·¢¹ýÀ´µÄÐÅºÅ1
				{
					  printf("OKKK");
					TIM_SetCompare1(TIM14,1100);
				  TIM_SetCompare1(TIM13,2000);
//					LED3=0;	 //´ò¿ª¿ÍÌüµÆ
					delay_ms(150);
					SYN_FrameInfo(2, "[v9][m1][t5]ÁÀÒÂ¼ÜÒÑ¾­´ò¿ªÁË");
					delay_ms(50);
					delay_ms(50);
				}
				if(USART2_RX_BUF[0]=='2')//½ÓÊÕµ½51µ¥Æ¬»ú·¢¹ýÀ´µÄÐÅºÅ2
				{
					 printf("OKKkK");
					TIM_SetCompare1(TIM14,1500);
				  TIM_SetCompare1(TIM13,1500);
//					LED3=1;  //¹Ø±Õ¿ÍÌüµÆ
					delay_ms(150);
					SYN_FrameInfo(2, "[v9][m1][t5]ÁÀÒÂ¼ÜÒÑ¾­¹Ø±ÕÁË");
					delay_ms(50);
					delay_ms(50);
				}
//				if(USART2_RX_BUF[0]=='3')//½ÓÊÕµ½51µ¥Æ¬»ú·¢¹ýÀ´µÄÐÅºÅ3
//				{
//          LED1=0;  //´ò¿ªÎÔÊÒµÆ
//					delay_ms(150); 
//					SYN_FrameInfo(2, "[v7][m1][t5]ÎÔÊÒµÆÒÑ¾­´ò¿ªÁË");
//					delay_ms(150);
//					delay_ms(150);
//				}
//				if(USART2_RX_BUF[0]=='4')//½ÓÊÕµ½51µ¥Æ¬»ú·¢¹ýÀ´µÄÐÅºÅ4
//				{
//					LED1=1;  //¹Ø±ÕÎÔÊÒµÆ
//					delay_ms(1500); 
//					SYN_FrameInfo(2, "[v7][m1][t5]ÎÔÊÒµÆÒÑ¾­¹Ø±ÕÁË");
//					delay_ms(150);
//					delay_ms(150);
//				}
//				if(USART2_RX_BUF[0]=='5')//½ÓÊÕµ½51µ¥Æ¬»ú·¢¹ýÀ´µÄÐÅºÅ5
//				{
//					LED0=0;  //´ò¿ª³ø·¿µÆ
//					delay_ms(150); 
//					SYN_FrameInfo(2, "[v7][m1][t5]³ø·¿µÆÒÑ¾­´ò¿ªÁË");
//					delay_ms(150);
//					delay_ms(150);
//				}
//				if(USART2_RX_BUF[0]=='6')//½ÓÊÕµ½51µ¥Æ¬»ú·¢¹ýÀ´µÄÐÅºÅ6
//				{
//					LED0=1;  //¹Ø±Õ³ø·¿µÆ
//					delay_ms(1500); 
//					SYN_FrameInfo(2, "[v7][m1][t5]³ø·¿µÆÒÑ¾­¹Ø±ÕÁË");
//					delay_ms(150);
//					delay_ms(150);
//				}
//				if(USART2_RX_BUF[0]=='7')//½ÓÊÕµ½51µ¥Æ¬»ú·¢¹ýÀ´µÄÐÅºÅ7
//				{
//					LED2=0;  //´ò¿ª¿Õµ÷
//					delay_ms(150); 
//					SYN_FrameInfo(2, "[v7][m1][t5]¿Õµ÷ÒÑ¾­´ò¿ªÁË");
//					delay_ms(150);
//					delay_ms(150);
//				}
//				if(USART2_RX_BUF[0]=='8')//½ÓÊÕµ½51µ¥Æ¬»ú·¢¹ýÀ´µÄÐÅºÅ8
//				{
//					LED2=1;  //¹Ø±Õ¿Õµ÷
//					delay_ms(150); 
//					SYN_FrameInfo(2, "[v7][m1][t5]¿Õµ÷ÒÑ¾­¹Ø±ÕÁË");
//					delay_ms(150);
//					delay_ms(150);
//				}
//				if(USART2_RX_BUF[0]=='9')//½ÓÊÕµ½51µ¥Æ¬»ú·¢¹ýÀ´µÄÐÅºÅ9
//				{
//					TIM_SetCompare1(TIM14,15);	//¿ªÃÅ
//					delay_ms(150); 
//					SYN_FrameInfo(2, "[v7][m1][t5]ÃÅÒÑ¾­´ò¿ªÁË");
//					delay_ms(150);
//					delay_ms(150);
//				}
//				if(USART2_RX_BUF[0]=='a')//½ÓÊÕµ½51µ¥Æ¬»ú·¢¹ýÀ´µÄÐÅºÅ10
//				{
//					TIM_SetCompare1(TIM14,5);	//¹ØÃÅ
//					delay_ms(150); 
//					SYN_FrameInfo(2, "[v7][m1][t5]ÃÅÒÑ¾­¹Ø±ÕÁË");
//					delay_ms(150);
//					delay_ms(150);
//				}
//				if(USART2_RX_BUF[0]=='b')//½ÓÊÕµ½51µ¥Æ¬»ú·¢¹ýÀ´µÄÐÅºÅ9
//				{
//					MOTOR_R=0;   //¿ª´°
//					MOTOR_L=1;
//				  delay_ms(1000);
//					MOTOR_R=0;
//					MOTOR_L=0;
//					delay_ms(1500); 
//					SYN_FrameInfo(2, "[v7][m1][t5]´°»§ÒÑ¾­´ò¿ªÁË");
//					delay_ms(1500);
//					delay_ms(1500);
//				}
//				if(USART2_RX_BUF[0]=='c')//½ÓÊÕµ½51µ¥Æ¬»ú·¢¹ýÀ´µÄÐÅºÅ10
//				{
//					MOTOR_R=1;  //¹Ø´°
//					MOTOR_L=0;
//				  delay_ms(1000);
//					MOTOR_R=0;
//					MOTOR_L=0;
//					delay_ms(1500); 
//					SYN_FrameInfo(2, "[v7][m1][t5]´°»§ÒÑ¾­¹Ø±ÕÁË");
//					delay_ms(1500);
//					delay_ms(1500);
//				}
			}
			 USART2_RX_STA=0;
			}	

//if(m==10)
//		 {
//			 m=0;
//			 
//			 //½«ÎÂ¶ÈºÍÊª¶ÈÉÏ´«µ½ÔÆÆ½Ì¨
//			 numToString(temperature);
//			 printf("temperature: %s,",strValue);
//			 LCD_ShowString(30,70,200,16,16,(u8 *)strValue);
//			 esp8266_str_data("temp",strValue);
//			 numToString(humidity);
//			 printf("humidity: %s\r\n\r\n",strValue);
//			 LCD_ShowString(30,90,200,16,16,(u8 *)strValue);
//			 esp8266_str_data("humi",strValue);
//			 
////			 LED_shan();
//		 }
//		delay_ms(10);
//		 m++;
	}											    
 }

