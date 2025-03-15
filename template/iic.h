#ifndef __IIC_H_
#define __IIC_H_


//单片机向EEPROM写一个字节的数据 
void EEPROM_W(unsigned char add,unsigned char SendData);

//单片机从EEPROM读一个字节的数据
unsigned char EEPROM_R(unsigned char add);

//单片机向8591写一个字节的数据,进行DA转换
void PCF8591_DAC(unsigned char SendData);

//单片机AD转换，输入参数为通道号（0-3）
unsigned char PCF8591_ADC(unsigned char channel);
/*
	通道号	0	1	2	3
	用途		外接	光敏	差分	电位器
*/


#endif
