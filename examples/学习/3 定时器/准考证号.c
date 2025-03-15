#include "stc15f2k60s2.h"
#include "absacc.h"
#include <stdio.h>

/////函数声明////
void externaldevice_IO(unsigned char beepdata,unsigned char relaydata);//IO模式，控制外部设备（蜂鸣器、继电器）
void externaldevice_MM(unsigned char beepdata,unsigned char relaydata);//MM模式，控制外部设备（蜂鸣器、继电器）
void delay_ms(unsigned int delaydata);//延时函数

void LED_IO(unsigned char LEDdata); //IO模式，控制LED
void LED_MM(unsigned char LEDdata); //MM模式，控制LED
void timer1_init(void);             //time1定时计数器配置为1ms的定时中断

////全局变量定义//////
unsigned int gTimer_1ms=0;  //全局变量，用于计时
unsigned int cnt_LED;
bit gFlag_10ms;             //全局变量，每10ms，该变量置1
bit gFlag_50ms;             //全局变量，每10ms，该变量置1
bit gFlag_100ms;            //全局变量，每10ms，该变量置1
bit gFlag_200ms;            //全局变量，每10ms，该变量置1
bit gFlag_250ms;            //全局变量，每10ms，该变量置1
bit gFlag_1000ms;           //全局变量，每10ms，该变量置1


///主函数//////
void main()
{
  unsigned char i=0;
  externaldevice_IO(0,0);
  LED_IO(0XFF);   //灭灯
  timer1_init();

  //实现LED秒闪烁
  while(1)
  {

    
  }
}
////////////////////////////////
void externaldevice_IO(unsigned char beepdata,unsigned char relaydata)//IO模式，控制外部设备（蜂鸣器、继电器）
{
  unsigned char extdev_data=0;
  if(beepdata) extdev_data|=0X40;
  if(relaydata) extdev_data|=0X10;
  EA = 0;//关闭中断总使能
  P0 = extdev_data;
  P2 = (P2 & 0X1F)|0XA0;//0b 101X XXXX 使138译码输入CBA为5，Y5输出低电平，573的LE为高锁存状态解除
  P0 = extdev_data;
  P2 &= 0X1F;//0b 000X XXXX //锁存数据
  EA = 1;//打开中断总使能
}
////////////////////////////////
void externaldevice_MM(unsigned char beepdata,unsigned char relaydata)//MM模式，控制外部设备（蜂鸣器、继电器）
{
  unsigned char extdev_data=0;
  if(beepdata) extdev_data|=0X40;
  if(relaydata) extdev_data|=0X10;
  EA = 0;//关闭中断总使能
  XBYTE[0XA000] = extdev_data;
  P2 &= 0X1F;//0b 000X XXXX //锁存数据
  EA = 1;//打开中断总使能
}
////////////////////////////////
void delay_ms(unsigned int delaydata)//延时函数
{
  unsigned int i;
  while(delaydata--)
    for(i=640;i>0;i--);
}

/////////////////////////////
void LED_IO(unsigned char LEDdata)//IO模式，控制LED
{
  
  EA = 0;//关闭中断总使能
  P0 = LEDdata;
  P2 = (P2 & 0X1F)|0X80;//0b 100X XXXX 使138译码输入CBA为5，Y5输出低电平，573的LE为高锁存状态解除
  P0 = LEDdata;
  P2 &= 0X1F;//0b 000X XXXX //锁存数据
  EA = 1;//打开中断总使能
}

void LED_MM(unsigned char LEDdata)//MM模式，控制LED
{
  EA = 0;//关闭中断总使能
  XBYTE[0X8000] = LEDdata;
  P2 &= 0X1F;//0b 000X XXXX //锁存数据
  EA = 1;//打开中断总使能
}

/////////////////////////////
void timer1_init(void)         //time1定时计数器配置为1ms的定时中断
{
	AUXR &= 0xBF;		//定时器时钟12T模式
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0x18;	  	//设置定时初始值
	TH1 = 0xFC;	  	//设置定时初始值
	TF1 = 0;	    	//清除TF1标志
	TR1 = 1;	    	//定时器1开始计时
  ET1 = 1;        //time1中断分允许
  EA = 1;         //中断总允许
}

void tm1_isr() interrupt 3
{
  gTimer_1ms++;
  cnt_LED++;
  if(gTimer_1ms == 60000)    gTimer_1ms=0;
  if(gTimer_1ms%10 == 0)    gFlag_10ms=1;
  if(gTimer_1ms%50 == 0)    gFlag_50ms=1;
  if(gTimer_1ms%100 == 0)   gFlag_100ms=1;
  if(gTimer_1ms%200 == 0)   gFlag_200ms=1;
  if(gTimer_1ms%250 == 0)   gFlag_250ms=1;
  if(gTimer_1ms%1000 == 0)  gFlag_1000ms=1;
}

