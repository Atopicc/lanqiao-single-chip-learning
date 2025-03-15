#include <STC15F2K60S2.H>

void Timer0_Init_MeasureFreq(void)
{
	AUXR &= 0x7F; // 定时器时钟12T模式,进入T0计数器的脉冲，周期1us
	TMOD &= 0xF0; // 设置定时器模式
	TMOD |= 0x04; // 设置成脉冲计数模式
	TL0 = 0;	  // 设置定时初始值
	TH0 = 0;	  // 设置定时初始值
	TF0 = 0;	  // 清除TF0标志
	TR0 = 1;	  // 定时器0开始计时
	ET0 = 0;	  // 关闭中断
}

// 定时器初始化500ms后执行
unsigned int GetFrequence()
{
	TR0 = 0;
	return ((TH0 << 8) | TL0) << 1;
}