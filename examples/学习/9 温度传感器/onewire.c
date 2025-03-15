/*
  程序说明: 单总线驱动程序
  软件环境: Keil uVision 4.10 
  硬件环境: CT107单片机综合实训平台(外部晶振12MHz) STC89C52RC单片机
  日    期: 2011-8-9
*/
#include "stc15f2k60s2.h"
#include "absacc.h"
#include "onewire.h"

unsigned int Temp_Value;		//存储温度值，实际温度的100倍
unsigned char Temp_Symbol;		//温度值符号，0正1负
float Temp_F;

sbit DQ = P1^4;  //单总线接口

//单总线延时函数
void Delay_OneWire(unsigned int t)  //STC89C52RC
{
	unsigned char i;
	while(t--)
	{
		for(i=0;i<12;i++);
	}
}

//通过单总线向DS18B20写一个字节
void Write_DS18B20(unsigned char dat)
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
		DQ = 0;
		DQ = dat&0x01;
		Delay_OneWire(5);
		DQ = 1;
		dat >>= 1;
	}
	Delay_OneWire(5);
}

//从DS18B20读取一个字节
unsigned char Read_DS18B20(void)
{
	unsigned char i;
	unsigned char dat;
  
	for(i=0;i<8;i++)
	{
		DQ = 0;
		dat >>= 1;
		DQ = 1;
		if(DQ)
		{
			dat |= 0x80;
		}	    
		Delay_OneWire(5);
	}
	return dat;
}

//DS18B20设备初始化
bit init_ds18b20(void)
{
  	bit initflag = 0;
  	
  	DQ = 1;
  	Delay_OneWire(12);
  	DQ = 0;
  	Delay_OneWire(80);
  	DQ = 1;
  	Delay_OneWire(10); 
    initflag = DQ;     
  	Delay_OneWire(5);
  
  	return initflag;
}
//读取DS18B20的温度值,温度值存储在全局变量中
void rd_temperature(void)
{
	unsigned char T_Low,T_High;
	unsigned int Tx_Int;
	init_ds18b20();				//初始化
	Write_DS18B20(0xCC);	    //跳过ROM
	Write_DS18B20(0x44); 		//启动转换
	Delay_OneWire(20);			//延时等待
	init_ds18b20();				//初始化
	Write_DS18B20(0xCC);		//跳过ROM
	Write_DS18B20(0xBE);		//发送读温度指令
	T_Low = Read_DS18B20();		//接收温度数据(9个字节)
	T_High = Read_DS18B20();
	Tx_Int = T_High;
	Tx_Int <<= 8;
	Tx_Int += T_Low;

	if(T_High & 0x80)//是否负温度
	{
		Temp_Symbol = 1;
		Tx_Int = ~Tx_Int + 1;
	}
	else
	{
		Temp_Symbol = 0;
	}
	Temp_F = Tx_Int*0.0625*100+0.5;
	Temp_Value = Temp_F;
}







