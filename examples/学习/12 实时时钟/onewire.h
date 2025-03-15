#ifndef __ONEWIRE_H
#define __ONEWIRE_H

extern unsigned int Temp_Value;		//存储温度值，实际温度的100倍
extern unsigned char Temp_Symbol;		//温度值符号，0正1负
extern float Temp_F;

//单总线延时函数
void Delay_OneWire(unsigned int t);
//通过单总线向DS18B20写一个字节
void Write_DS18B20(unsigned char dat);
//从DS18B20读取一个字节
unsigned char Read_DS18B20(void);
//DS18B20设备初始化
bit init_ds18b20(void);
//读取DS18B20的温度值,温度值存储在全局变量中
void rd_temperature(void);  

#endif
