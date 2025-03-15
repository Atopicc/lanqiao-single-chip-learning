#include <STC15F2K60S2.H>
#include "btn.h"

unsigned char Key_TempBTN = 0,Key_DownBTN = 0,Key_UpBTN = 0,key_long_press = 0;
unsigned char Key_OldBTN = 0,long_press_value = 0;
unsigned int key_long_press_count = 0;
unsigned char key_long_press_flag = 0;

unsigned char GetKeyBTN()
{
	unsigned char temp = 0;
	P3 |= 0x0F;
	temp = P3&0x0F;
	
	switch(~temp)
	{
		case 0xF1:return 7;break;
		case 0xF2:return 6;break;
		case 0xF4:return 5;break;
		case 0xF8:return 4;break;
		default:return 0;break;
	}
}

void btnStatus(void)
{
	Key_TempBTN = GetKeyBTN();
	Key_DownBTN = Key_TempBTN & (Key_OldBTN ^ Key_TempBTN);   //按下为按键值，其它为0
	Key_UpBTN  = ~Key_TempBTN & (Key_OldBTN ^ Key_TempBTN);  //松手为抬起前的按键值，其他为0 
	Key_OldBTN = Key_TempBTN;

	if(Key_DownBTN)
	{
		key_long_press_flag = 1;
		long_press_value = Key_DownBTN;
		key_long_press_count = 0;
	}
	if(key_long_press_flag)
	{
		key_long_press_count++;
	}
	if(Key_UpBTN)
	{
		key_long_press_flag = 0;
		key_long_press_count = 0;
	}
	if(key_long_press_count > (long_press_time/scan_time))
	{
		key_long_press = long_press_value;
	}else
	{
		key_long_press = 0;
	}
}

