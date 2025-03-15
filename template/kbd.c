#include <STC15F2K60S2.H>
#include "kbd.h"

//unsigned char trg = 0;
//unsigned char count = 0;

unsigned char Key_Temp;
unsigned char Key_Down = 0;
unsigned char Key_Up = 0;
unsigned char Key_Old;

unsigned char Key_Read()
{
	unsigned int KeyDataScan; 
	unsigned char key_value;
	
	P44 = 0; P42 = 1; P35 = 1; P34 = 1;
	KeyDataScan = P3&0x0F;
	
	P44 = 1; P42 = 0; P35 = 1; P34 = 1;
	KeyDataScan =(KeyDataScan<<4)|(P3&0x0F);
	
	P44 = 1; P42 = 1; P35 = 0; P34 = 1;
	KeyDataScan =(KeyDataScan<<4)|(P3&0x0F);	
	
	P44 = 1; P42 = 1; P35 = 1; P34 = 0;
	KeyDataScan =(KeyDataScan<<4)|(P3&0x0F);	
	
	switch(~KeyDataScan)    //取反，按键按下是0，不按是1 
	{
		case 0x8000: key_value = 4; break;
		case 0x4000: key_value = 5; break;
		case 0x2000: key_value = 6; break;
		case 0x1000: key_value = 7; break;
		
		case 0x0800: key_value = 8; break;
		case 0x0400: key_value = 9; break;
		case 0x0200: key_value = 10; break;
		case 0x0100: key_value = 11; break;
		
		case 0x0080: key_value = 12; break;
		case 0x0040: key_value = 13; break;
		case 0x0020: key_value = 14; break;
		case 0x0010: key_value = 15; break;
		
		case 0x0008: key_value = 16; break;
		case 0x0004: key_value = 17; break;
		case 0x0002: key_value = 18; break;
		case 0x0001: key_value = 19; break;
		
		default: key_value = 0;
	}
	
	return key_value;
}

void kbdProcess(void)
{
	Key_Temp = Key_Read();
	Key_Down = Key_Temp & (Key_Old ^ Key_Temp);   //按下为按键值，其它为0
	Key_Up  = ~Key_Temp & (Key_Old ^ Key_Temp);  //松手为抬起前的按键值，其他为0 
	Key_Old = Key_Temp;
}

//void KEY_Status()
//{
//	unsigned char read_data = (Key_Read())^(0xff);
//	trg = read_data&(read_data^cont);
//	cont = read_data;
//}

//unsigned char KBD(void)
//{
//	static unsigned key_val;
//	P3 = 0xf0;P42 = 1;P44 = 1;
//		if(P34 == 0) key_val = 0xe0;
//		if(P35 == 0) key_val = 0xd0;
//		if(P42 == 0) key_val = 0xb0;
//		if(P44 == 0) key_val = 0x70;
//	P3 = 0x0f;P42 = 0;P44 = 0;	
//		if(P30 == 0) key_val = key_val|0x0e;
//		if(P31 == 0) key_val = key_val|0x0d;
//		if(P32 == 0) key_val = key_val|0x0b;
//		if(P33 == 0) key_val = key_val|0x07;	
//	return key_val;
//}
// 
//void KBD_key_read(void)      //矩阵键盘
//{
//	unsigned char read_val;
//	read_val = (KBD()) ^0xff;
//	trg = read_val & (read_val ^count );
//	count =read_val;
//}


