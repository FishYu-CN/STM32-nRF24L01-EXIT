#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	 
#include "24l01.h" 	 
#include "touch.h"
//ALIENTEK战舰STM32开发板实验32
//无线通信 实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司  
extern u8 nrf_flag,tmp_buf[];
 int main(void)
 {	 
	u8 key,mode;
	u16 t=0;			 		    
	delay_init();	    	 //延时函数初始化	  
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(9600);	 	//串口初始化为9600
 	LED_Init();			     //LED端口初始化
	LCD_Init();	//初始化LCD
	KEY_Init();	 //按键初始化	
	tp_dev.init();
	tp_dev.xfac=0.0662251636;
	tp_dev.yfac=0.0861273482;
	tp_dev.xoff=0xFFF4;
	tp_dev.yoff=0xFFEA;
	tp_dev.touchtype=0x00;	
	LCD_Clear(WHITE);
 	NRF24L01_Init();    	//初始化NRF24L01 

	POINT_COLOR=RED;//设置字体为红色 
	LCD_ShowString(60,50,200,16,16,"WarShip STM32");	
	LCD_ShowString(60,70,200,16,16,"NRF24L01 TEST");	
	LCD_ShowString(60,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(60,110,200,16,16,"2012/9/13");		  
 	while(NRF24L01_Check())	//检查NRF24L01是否在位.	
	{
		LCD_ShowString(60,130,200,16,16,"NRF24L01 Error");
		delay_ms(200);
		LCD_Fill(60,130,239,130+16,WHITE);
 		delay_ms(200);
	}								   
	LCD_ShowString(60,130,200,16,16,"NRF24L01 OK");
	while(1)//在该部分确定进入哪个模式!
	{
	 	key=KEY_Scan(0);
		if(key==KEY_RIGHT)
		{
			mode=0;   
			break;
		}else if(key==KEY_UP)
		{
			mode=1;
			break;
		}

		t++;		
		if(t==100)LCD_ShowString(10,150,230,16,16,"KEY0:RX_Mode  KEY1:TX_Mode"); //闪烁显示提示信息
 		if(t==200)
		{	
			LCD_Fill(10,150,230,150+16,WHITE);
			t=0; 
		}
		
		tp_dev.scan(0); 
		if(tp_dev.sta&TP_PRES_DOWN)			//触摸屏被按下
		{	
			LED1=!LED1;
		 	if(tp_dev.x[0]<lcddev.width&&tp_dev.y[0]<lcddev.height)
			{	
				if(tp_dev.x[0]>(10)&&tp_dev.x[0]<(10+8*12)&&tp_dev.y[0]>150&&tp_dev.y[0]<166)
					{			
						mode=0;   
					break;
					}
				else if(tp_dev.x[0]>(10+8*12)&&tp_dev.x[0]<(10+8*27)&&tp_dev.y[0]>150&&tp_dev.y[0]<166)
					{			
						mode=1;   
					break;
					}			   
			}
		} 
		delay_ms(5);	//没有按键按下的时候 	    
//		if(key==KEY_RIGHT)	//KEY_RIGHT按下,则执行校准程序
//		{
//			LCD_Clear(WHITE);//清屏
//		    TP_Adjust();  //屏幕校准 
//			TP_Save_Adjdata();	 
//			Load_Drow_Dialog();
//		}
	} 
 	LCD_Fill(10,150,240,166,WHITE);//清空上面的显示		  
 	POINT_COLOR=BLUE;//设置字体为蓝色	   
	if(mode==0)//RX模式
	{
		LCD_ShowString(60,150,200,16,16,"NRF24L01 RX_Mode");	
		LCD_ShowString(60,170,200,16,16,"Received DATA:");	
		NRF24L01_RX_Mode();		  

		while(1)
		{	  		    		    				 
			if(nrf_flag==0x40)//一旦接收到信息,则显示出来.
			{
				for(t=0;t<tmp_buf[0];t++)
				{
						LCD_ShowChar(0+t*8,190,tmp_buf[t+1],16,0);
				}
			 nrf_flag=0;
			}else delay_us(100);	   
			t++;
			if(t==10000)//大约1s钟改变一次状态
			{
				t=0;
				LED1=!LED1;
			} 				    
		};	
	}else//TX模式
	{							    
		LCD_ShowString(60,150,200,16,16,"NRF24L01 TX_Mode");	
		NRF24L01_TX_Mode();
		tmp_buf[0]=1;//加入结束符	
		tmp_buf[1]='a';//加入结束符
		while(1)
		{	   
			NRF24L01_TxPacket(tmp_buf);   				 
			if((nrf_flag&0x20)>0)
			{
				LCD_ShowString(60,170,239,32,16,"Sended DATA:");	
				for(t=0;t<tmp_buf[0];t++)
				{
						LCD_ShowChar(0+t*8,190,tmp_buf[t+1],16,0);
				}
					tmp_buf[1]=='z'?tmp_buf[1] = 'a': tmp_buf[1]++;   
			}else
			{										   	
 				LCD_ShowString(60,170,239,32,16,"Send Failed "); 
				LCD_Fill(0,188,240,218,WHITE);//清空上面的显示			   
			};
			LED1=!LED1;
			delay_ms(1500);				    
		};
	}     
}


