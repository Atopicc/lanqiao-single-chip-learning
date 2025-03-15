/*
  ����˵��: DS1302��������
  �������: Keil uVision 4.10 
  Ӳ������: CT107��Ƭ���ۺ�ʵѵƽ̨ 8051��12MHz
  ��    ��: 2011-8-9
*/

#include "stc15f2k60s2.h"
#include <intrins.h>
#include "ds1302.h"

#define Add_WriteSeconds 0x80		//д�����ַ
#define Add_ReadSeconds 0x81	   	//��ȡ���ַ
#define Add_WriteMinutes 0x82		//д��ֵ�ַ
#define Add_ReadMinutes 0x83 		//��ȡ�ֵ�ַ
#define Add_WriteHours 0x84 		//д��ʱ��ַ
#define Add_ReadHours 0x85 			//��ȡʱ��ַ
#define Add_WriteDate 0x86	 		//д���յ�ַ
#define Add_ReadDate 0x87 			//��ȡ�յ�ַ
#define Add_WriteMonth 0x88 		//д���µ�ַ
#define Add_ReadMonth 0x89			//��ȡ�µ�ַ
#define Add_WriteDay 0x8A 			//д�����ڵ�ַ
#define Add_ReadDay 0x8B			//��ȡ���ڵ�ַ
#define Add_WriteYear 0x8C 			//д�����ַ
#define Add_ReadYear 0x8D 			//��ȡ���ַ
#define Add_WriteWP 0x8E 			//д������λ��ַ
#define Add_ReadWP 0x8F 			//��ȡ����λ��ַ


unsigned char SetRTC_Data[7] = {0x23,0x12,0x14,0x04,0x08,0x53,0x00}; //�� �� �� ���� ʱ �� ��
unsigned char ReadRTC_Data[7] = {0x23,0x12,0x14,0x04,0x08,0x53,0x00};//�� �� �� ���� ʱ �� ��

sbit SCK=P1^7;		
sbit SDA=P2^3;		
sbit RST = P1^3;   // DS1302��λ												

void Write_Ds1302(unsigned  char temp) 
{
	unsigned char i;
	for (i=0;i<8;i++)     	
	{ 
		SCK=0;
		SDA=temp&0x01;
		temp>>=1; 
		SCK=1;
	}
}   

void Write_Ds1302_Byte( unsigned char address,unsigned char dat )     
{
 	RST=0;	_nop_();
 	SCK=0;	_nop_();
 	RST=1; 	_nop_();  
 	Write_Ds1302(address);	
 	Write_Ds1302(dat);		
 	RST=0; 
}

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

void SetRTC(void)
{
	Write_Ds1302_Byte(Add_WriteWP,0x00);
	Write_Ds1302_Byte(Add_WriteYear,SetRTC_Data[0]);
	Write_Ds1302_Byte(Add_WriteMonth,SetRTC_Data[1]);
	Write_Ds1302_Byte(Add_WriteDate,SetRTC_Data[2]);
	Write_Ds1302_Byte(Add_WriteDay,SetRTC_Data[3]);
	Write_Ds1302_Byte(Add_WriteHours,SetRTC_Data[4]);
	Write_Ds1302_Byte(Add_WriteMinutes,SetRTC_Data[5]);
	Write_Ds1302_Byte(Add_WriteSeconds,SetRTC_Data[6]);
	Write_Ds1302_Byte(Add_WriteWP,0x80);
}

void ReadRTC(void)
{
	ReadRTC_Data[0]=Read_Ds1302_Byte(Add_ReadYear);
	ReadRTC_Data[1]=Read_Ds1302_Byte(Add_ReadMonth);
	ReadRTC_Data[2]=Read_Ds1302_Byte(Add_ReadDate);
	ReadRTC_Data[3]=Read_Ds1302_Byte(Add_ReadDay);
	ReadRTC_Data[4]=Read_Ds1302_Byte(Add_ReadHours);
	ReadRTC_Data[5]=Read_Ds1302_Byte(Add_ReadMinutes);
	ReadRTC_Data[6]=Read_Ds1302_Byte(Add_ReadSeconds);
}