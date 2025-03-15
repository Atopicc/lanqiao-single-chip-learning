/*	# 	DS1302代码片段说明
	1. 	本文件夹中提供的驱动代码供参赛选手完成程序设计参考。
	2. 	参赛选手可以自行编写相关代码或以该代码为基础，根据所选单片机类型、运行速度和试题
		中对单片机时钟频率的要求，进行代码调试和修改。
*/								
/******以下为自己补充内容*******/

#include <STC15F2K60S2.H>
#include "intrins.h"
#include "ds1302.h"

/*接口定义*/
sbit SCK=P1^7;
sbit SDA=P2^3;
sbit RST=P1^3;

unsigned char SetRTC_Data[7] = {0x23,0x08,0x13,0x06,0x23,0x59,0x54}; 
								//年   月    日   星期  时   分    秒
unsigned char ReadRTC_Data[7] = {0x23,0x08,0x13,0x06,0x11,0x20,0x55};
								//年   月    日   星期  时   分    秒

/****************************/

void Write_Ds1302(unsigned  char temp) 
{
	unsigned char i;
	for (i=0;i<8;i++)     	
	{ 
		SCK = 0;
		SDA = temp&0x01;
		temp>>=1; 
		SCK=1;
	}
}   

//
void Write_Ds1302_Byte( unsigned char address,unsigned char dat )     
{
 	RST=0;	_nop_();
 	SCK=0;	_nop_();
 	RST=1; 	_nop_();  
 	Write_Ds1302(address);	
 	Write_Ds1302(dat);		
 	RST=0; 
}

//
unsigned char Read_Ds1302_Byte ( unsigned char address )
{
 	unsigned char i,temp=0x00;
 	RST=0;	_nop_();
 	SCK=0;	_nop_();
 	RST=1;	_nop_();
 	Write_Ds1302(address);
 	for (i=0;i<8;i++) 	
 	{		
		SCK=0;
		temp>>=1;	
 		if(SDA)
 		temp|=0x80;	
 		SCK=1;
	} 
 	RST=0;	_nop_();
 	SCK=0;	_nop_();
	SCK=1;	_nop_();
	SDA=0;	_nop_();
	SDA=1;	_nop_();
	return (temp);			
}

/******以下为自己补充内容*******/
/*
	DS1302地址说明：
		从0x80开始到0x8F，16个地址依次为：
		秒	分	时	日	月	星期	年	允许
	写入	80	82	84	86	88	8A	8C	8E	
	读取	81	83	85	87	89	8B	8D	8F
*/
void SetRTC(void)
{
	Write_Ds1302_Byte(0x8E,0x00);
	Write_Ds1302_Byte(0x8C,SetRTC_Data[0]);
	Write_Ds1302_Byte(0x88,SetRTC_Data[1]);
	Write_Ds1302_Byte(0x86,SetRTC_Data[2]);
	Write_Ds1302_Byte(0x8A,SetRTC_Data[3]);
	Write_Ds1302_Byte(0x84,SetRTC_Data[4]);
	Write_Ds1302_Byte(0x82,SetRTC_Data[5]);
	Write_Ds1302_Byte(0x80,SetRTC_Data[6]);
	Write_Ds1302_Byte(0x8E,0x80);
}

void ReadRTC(void)
{
	ReadRTC_Data[0] = Read_Ds1302_Byte(0x8D);
	ReadRTC_Data[1] = Read_Ds1302_Byte(0x89);
	ReadRTC_Data[2] = Read_Ds1302_Byte(0x87);
	ReadRTC_Data[3] = Read_Ds1302_Byte(0x8B);
	ReadRTC_Data[4] = Read_Ds1302_Byte(0x85);
	ReadRTC_Data[5] = Read_Ds1302_Byte(0x83);
	ReadRTC_Data[6] = Read_Ds1302_Byte(0x81);
}

/****************************/