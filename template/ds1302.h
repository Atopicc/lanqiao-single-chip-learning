#ifndef __DS1302_H_
#define __DS1302_H_

extern unsigned char SetRTC_Data[7];
extern unsigned char ReadRTC_Data[7];

//设置时间
void SetRTC(void);
//读取时间
void ReadRTC(void);

#endif
