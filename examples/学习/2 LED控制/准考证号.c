#include "stc15f2k60s2.h"
#include "absacc.h"

/////函数声明////
void externaldevice_IO(unsigned char beepdata,unsigned char relaydata);//IO模式，控制外部设备（蜂鸣器、继电器）
void externaldevice_MM(unsigned char beepdata,unsigned char relaydata);//MM模式，控制外部设备（蜂鸣器、继电器）
void delay_ms(unsigned int delaydata);//延时函数

void LED_IO(unsigned char LEDdata);//IO模式，控制LED
void LED_MM(unsigned char LEDdata);//MM模式，控制LED

////全局变量定义//////

///主函数//////
void main()
{
  unsigned char i=0;
  externaldevice_IO(0,0);
  //实现以秒为间隔，LED流水灯
  while(1)
  {
    LED_IO(~(0x01<<i));
    delay_ms(600);
    i++;
    if(i>=8)
    i=0;
    
  }
}
////////////////////////////////
void externaldevice_IO(unsigned char beepdata,unsigned char relaydata)//IO模式，控制外部设备（蜂鸣器、继电器）
{
  unsigned char extdev_data=0;
  if(beepdata) extdev_data|=0X40;
  if(relaydata) extdev_data|=0X10;
  EA = 0;//关闭中断总使能
  P0 = extdev_data;
  P2 = (P2 & 0X1F)|0XA0;//0b 101X XXXX 使138译码输入CBA为5，Y5输出低电平，573的LE为高锁存状态解除
  P0 = extdev_data;
  P2 &= 0X1F;//0b 000X XXXX //锁存数据
  EA = 1;//打开中断总使能
}
////////////////////////////////
void externaldevice_MM(unsigned char beepdata,unsigned char relaydata)//MM模式，控制外部设备（蜂鸣器、继电器）
{
  unsigned char extdev_data=0;
  if(beepdata) extdev_data|=0X40;
  if(relaydata) extdev_data|=0X10;
  EA = 0;//关闭中断总使能
  XBYTE[0xA000] = extdev_data;
  P2 &= 0X1F;//0b 000X XXXX //锁存数据
  EA = 1;//打开中断总使能
}
////////////////////////////////
void delay_ms(unsigned int delaydata)//延时函数
{
  unsigned int i;
  while(delaydata--)
    for(i=640;i>0;i--);
}

/////////////////////////////
void LED_IO(unsigned char LEDdata)//IO模式，控制LED
{
  
  EA = 0;//关闭中断总使能
  P0 = LEDdata;
  P2 = (P2 & 0X1F)|0X80;//0b 100X XXXX 使138译码输入CBA为5，Y5输出低电平，573的LE为高锁存状态解除
  P0 = LEDdata;
  P2 &= 0X1F;//0b 000X XXXX //锁存数据
  EA = 1;//打开中断总使能
}

void LED_MM(unsigned char LEDdata)//MM模式，控制LED
{
  EA = 0;//关闭中断总使能
  XBYTE[0x8000] = LEDdata;
  P2 &= 0X1F;//0b 000X XXXX //锁存数据
  EA = 1;//打开中断总使能
}