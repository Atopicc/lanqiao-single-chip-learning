#ifndef _IIC_H
#define _IIC_H

void IIC_Start(void); 
void IIC_Stop(void);  
bit IIC_WaitAck(void);  
void IIC_SendAck(bit ackbit); 
void IIC_SendByte(unsigned char byt); 
unsigned char IIC_RecByte(void);
void write_EEPROM(unsigned char add,unsigned char EEPData); //单片机向EEPROM写一个字节的数据 
unsigned char read_EEPROM(unsigned char add);  //单片机向EEPROM读一个字节的数据
void DAC_PCF8591(unsigned char DACData); //单片机向8591写一个字节的数据,进行DA转换
unsigned char ADC_PCF8591(unsigned char channel);  //单片机AD转换，输入参数为通道号（0-3）


#endif