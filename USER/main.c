#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	 
#include "24l01.h" 	 
#include "touch.h"
//ALIENTEKս��STM32������ʵ��32
//����ͨ�� ʵ��  
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾  
extern u8 nrf_flag,tmp_buf[];
 int main(void)
 {	 
	u8 key,mode;
	u16 t=0;			 		    
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(9600);	 	//���ڳ�ʼ��Ϊ9600
 	LED_Init();			     //LED�˿ڳ�ʼ��
	LCD_Init();	//��ʼ��LCD
	KEY_Init();	 //������ʼ��	
	tp_dev.init();
	tp_dev.xfac=0.0662251636;
	tp_dev.yfac=0.0861273482;
	tp_dev.xoff=0xFFF4;
	tp_dev.yoff=0xFFEA;
	tp_dev.touchtype=0x00;	
	LCD_Clear(WHITE);
 	NRF24L01_Init();    	//��ʼ��NRF24L01 

	POINT_COLOR=RED;//��������Ϊ��ɫ 
	LCD_ShowString(60,50,200,16,16,"WarShip STM32");	
	LCD_ShowString(60,70,200,16,16,"NRF24L01 TEST");	
	LCD_ShowString(60,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(60,110,200,16,16,"2012/9/13");		  
 	while(NRF24L01_Check())	//���NRF24L01�Ƿ���λ.	
	{
		LCD_ShowString(60,130,200,16,16,"NRF24L01 Error");
		delay_ms(200);
		LCD_Fill(60,130,239,130+16,WHITE);
 		delay_ms(200);
	}								   
	LCD_ShowString(60,130,200,16,16,"NRF24L01 OK");
	while(1)//�ڸò���ȷ�������ĸ�ģʽ!
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
		if(t==100)LCD_ShowString(10,150,230,16,16,"KEY0:RX_Mode  KEY1:TX_Mode"); //��˸��ʾ��ʾ��Ϣ
 		if(t==200)
		{	
			LCD_Fill(10,150,230,150+16,WHITE);
			t=0; 
		}
		
		tp_dev.scan(0); 
		if(tp_dev.sta&TP_PRES_DOWN)			//������������
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
		delay_ms(5);	//û�а������µ�ʱ�� 	    
//		if(key==KEY_RIGHT)	//KEY_RIGHT����,��ִ��У׼����
//		{
//			LCD_Clear(WHITE);//����
//		    TP_Adjust();  //��ĻУ׼ 
//			TP_Save_Adjdata();	 
//			Load_Drow_Dialog();
//		}
	} 
 	LCD_Fill(10,150,240,166,WHITE);//����������ʾ		  
 	POINT_COLOR=BLUE;//��������Ϊ��ɫ	   
	if(mode==0)//RXģʽ
	{
		LCD_ShowString(60,150,200,16,16,"NRF24L01 RX_Mode");	
		LCD_ShowString(60,170,200,16,16,"Received DATA:");	
		NRF24L01_RX_Mode();		  

		while(1)
		{	  		    		    				 
			if(nrf_flag==0x40)//һ�����յ���Ϣ,����ʾ����.
			{
				for(t=0;t<tmp_buf[0];t++)
				{
						LCD_ShowChar(0+t*8,190,tmp_buf[t+1],16,0);
				}
			 nrf_flag=0;
			}else delay_us(100);	   
			t++;
			if(t==10000)//��Լ1s�Ӹı�һ��״̬
			{
				t=0;
				LED1=!LED1;
			} 				    
		};	
	}else//TXģʽ
	{							    
		LCD_ShowString(60,150,200,16,16,"NRF24L01 TX_Mode");	
		NRF24L01_TX_Mode();
		tmp_buf[0]=1;//���������	
		tmp_buf[1]='a';//���������
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
				LCD_Fill(0,188,240,218,WHITE);//����������ʾ			   
			};
			LED1=!LED1;
			delay_ms(1500);				    
		};
	}     
}


