#include <STC15F2K60S2.H>
#include "intrins.h"
#include "sonic.h"

#define SOMENOP {_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();}
#define SonicSpeed 0.0343	 //厘米每微秒

sbit sonicTX = P1^0;
sbit sonicRX = P1^1;

void SendWave(void);

void Timer0Init_UtralSonic()
{
	AUXR &= 0x7F;			//定时器时钟12T模式,进入T0计数器的脉冲，周期1us
	TMOD &= 0xF0;			//设置定时器模式
	TL0 = 0;		  		//设置定时初始值
	TH0 = 0;			  	//设置定时初始值
	TF0 = 0;		  		//清除TF0标志
	TR0 = 1;			  	//定时器0开始计时
}

void SendWave(void)
{
	unsigned char i;
	for(i=0; i<8; i++)
	{
		sonicTX = 0;
		SOMENOP;SOMENOP;SOMENOP;SOMENOP;SOMENOP;
		SOMENOP;SOMENOP;SOMENOP;SOMENOP;SOMENOP;
		sonicTX = 1;
		SOMENOP;SOMENOP;SOMENOP;SOMENOP;SOMENOP;
		SOMENOP;SOMENOP;SOMENOP;SOMENOP;SOMENOP;
	}
}

unsigned int GetDistence(void)
{
	unsigned int time,distence;
	SendWave();
	Timer0Init_UtralSonic();
	while((sonicRX) && (TH0<80));
	TR0 = 0;
	if(TH0>=79)
	{
		distence = 999;
	}else{
		time = TH0*256+TL0; //单位微秒
		distence = (unsigned int)(0.5 * time * SonicSpeed);
	}
	return distence;
}
