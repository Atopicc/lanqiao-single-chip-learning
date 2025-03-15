/*	# 	单总线代码片段说明
	1. 	本文件夹中提供的驱动代码供参赛选手完成程序设计参考。
	2. 	参赛选手可以自行编写相关代码或以该代码为基础，根据所选单片机类型、运行速度和试题
		中对单片机时钟频率的要求，进行代码调试和修改。
*/

/************以下自己补充***************/

#include "stc15f2k60s2.h"
#include "absacc.h" //注意引用的头文件
#include "onewire.h"

sbit DQ=P1^4;

#define SKIP 0xCC
#define START 0x44
#define READ 0xBE

/***************************************/

//
void Delay_OneWire(unsigned int t)  
{
	unsigned char i;
	while(t--){
		for(i=0;i<12;i++);
	}
}

//
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

//
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

//
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

/************以下自己补充***************/
/*
	DS18B20读取时序：
		初始化->跳过ROM->启动转换->延时等待->
		初始化->跳过ROM->读取温度->接收温度
*/

float GetTemputerature(void)
{
	unsigned char TemputeratureHigh,TemputeratureLow;
	unsigned int TemputeratureInitial;
	init_ds18b20();
	Write_DS18B20(SKIP);
	Write_DS18B20(START);
	
	init_ds18b20();
	Write_DS18B20(SKIP);
	Write_DS18B20(READ);
	TemputeratureLow = Read_DS18B20();
	TemputeratureHigh = Read_DS18B20();
	TemputeratureInitial = (TemputeratureHigh << 8) | TemputeratureLow;
	/*以下为消除温度读取85代码*/
//	if(TemputeratureHigh >= 0x05)
//	{return 0x19;}else 
	return TemputeratureInitial * 0.0625;
}

/**************************************/
