#ifndef __DS1302_H
#define __DS1302_H

extern unsigned char SetRTC_Data[7];
extern unsigned char ReadRTC_Data[7];

void Write_Ds1302(unsigned char temp);
void Write_Ds1302_Byte( unsigned char address,unsigned char dat );
unsigned char Read_Ds1302_Byte( unsigned char address );

//设置ds1302的时间
void SetRTC(void);
//读取ds1302的时间
void ReadRTC(void);

#endif
