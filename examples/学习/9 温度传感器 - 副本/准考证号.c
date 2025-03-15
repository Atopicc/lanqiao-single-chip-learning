#include "stc15f2k60s2.h"
#include "absacc.h"
#include <stdio.h>
#include "iic.h"
#include "onewire.h"

/////函数声明////
void externaldevice_IO(unsigned char beepdata,unsigned char relaydata);//IO模式，控制外部设备（蜂鸣器、继电器）
void externaldevice_MM(unsigned char beepdata,unsigned char relaydata);//MM模式，控制外部设备（蜂鸣器、继电器）
void delay_ms(unsigned int delaydata);//延时函数

void LED_IO(unsigned char LEDdata);      //IO模式，控制LED
void LED_MM(unsigned char LEDdata);      //MM模式，控制LED
void timer1_init(void);                  //time1定时计数器配置为1ms的定时中断
void display_IO(void);                   //数码管动态刷新函数
void display_MM(void);                  //数码管动态刷新函数
unsigned char BTN_KeyBoardScan(void);   //独立按键扫描程序，无有效按键按下时，返回0xFF;按键按下，返回对应序号；按键长按时，过一段时间，返回对应序号+10；松手时，返回对应序号+20。（4567）
unsigned char KBD_KeyBoardScan(void);   //矩阵按键扫描程序，无有效按键按下时，返回0xFF;按键按下，返回对应序号；按键长按时，过一段时间，返回对应序号+20；松手时，返回对应序号+40。（4-19）
////全局变量定义//////
unsigned int gTimer_1ms=0;  //全局变量，用于计时
bit gFlag_10ms;             //全局变量，每10ms，该变量置1
bit gFlag_50ms;             //全局变量，每50ms，该变量置1
bit gFlag_100ms;            //全局变量，每100ms，该变量置1
bit gFlag_200ms;            //全局变量，每200ms，该变量置1
bit gFlag_250ms;            //全局变量，每250ms，该变量置1
bit gFlag_1000ms;           //全局变量，每1000ms，该变量置1
unsigned char dispbuf[8] = {16,16,16,16,16,16,16,16}; //显示缓冲区
/*************  本地常量声明    **************/
unsigned char code disptab[]={                       //标准字库(共阴)
//   0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
    0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,
//black  -     H    J    K    L    N    o   P    U     t    G    Q    r   M    y
    0x00,0x40,0x76,0x1E,0x70,0x38,0x37,0x5C,0x73,0x3E,0x78,0x3d,0x67,0x50,0x37,0x6e,
   //0.   1.   2.  3.    4.    5.  6.   7.   8.    9.   -1 
    0xBF,0x86,0xDB,0xCF,0xE6,0xED,0xFD,0x87,0xFF,0xEF,0x46};
unsigned int temp_rd[6];
unsigned int temp_max,temp_min,temp_average;

////////////DS18B20变量//////////
/*extern unsigned int Temp_Value;		//存储温度值，实际温度的100倍
extern unsigned char Temp_Symbol;		//温度值符号，0正1负
extern float Temp_F;*/


///主函数//////
void main()
{
  unsigned char i=0;
  unsigned char key_value = 0xFF;
  unsigned char AD1_value_Now = 0;
  unsigned char AD3_value_Now = 0;

  externaldevice_IO(0,0);
  LED_IO(0XFF);   //灭灯
  timer1_init();

  //温度传感器
  while(1)
  {
        unsigned char i;
        if(gFlag_200ms)
        {
            gFlag_200ms = 0;
            for(i = 0;i<5;i++)
            {
                do
                {
                    rd_temperature();
                    temp_rd[i] = Temp_Value;
                    delay_ms(5);
                }while(Temp_Value<50);
            }
            for(i = 0;i<5;i++)
            {
                temp_max = 0;
                temp_min = 0xFFFF;
                if(temp_rd[i] > temp_max) temp_max = temp_rd[i];
                if(temp_rd[i] < temp_min) temp_min = temp_rd[i];
                temp_rd[6] += temp_rd[i];
            }
            temp_rd[6] = temp_rd[6]-temp_max-temp_min;
            temp_average = temp_rd[6]/3;
            if(temp_average < 7500)
            {
                if(temp_average >= 1000) dispbuf[0] = temp_average%10000/1000;
                else dispbuf[0] = 16;
                dispbuf[1] = temp_average%1000/100 + 32;
                dispbuf[2] = temp_average%100/10;
                dispbuf[3] = temp_average%10;
            }
        }
  }

  //ADDA
  while(1)
  {
    if(gFlag_200ms)
    {
      gFlag_200ms = 0;
      ADC_PCF8591(0x01);
      ADC_PCF8591(0x01);
      AD1_value_Now = ADC_PCF8591(0x01);
      dispbuf[0] = AD1_value_Now/100;
      dispbuf[1] = AD1_value_Now%100/10;
      dispbuf[2] = AD1_value_Now%10;
      ADC_PCF8591(0x03);
      ADC_PCF8591(0x03);
      AD3_value_Now = ADC_PCF8591(0x03);
      dispbuf[5] = AD3_value_Now/100;
      dispbuf[6] = AD3_value_Now%100/10;
      dispbuf[7] = AD3_value_Now%10;
      DAC_PCF8591(AD3_value_Now);
    } 
  }




//I2C任务
  EA = 0;
  i = read_EEPROM(0x00);
  if(i>=100)  dispbuf[5] = i/100;
  else dispbuf[5] = 16;
  if(i >= 10) dispbuf[6] = i%100/10;
  else dispbuf[6] = 16;
  dispbuf[7] = i%10;
  i++;
  write_EEPROM(0x00,i);
  EA = 1;
  while(1);


//独立按键任务
  while(1)
  {
    if(gFlag_10ms)
    {
      gFlag_10ms = 0;
      key_value = KBD_KeyBoardScan();
    }
    if(key_value != 0xFF)
    {
      dispbuf[0] = key_value/10;
      dispbuf[1] = key_value%10;
      switch(key_value)
      {
        case 7:i++; break;
        case 11:i--; break;
        case 15:i += 2; break;
        case 19:i -= 2; break;
        default:break;
      }
      key_value = 0xFF;
      dispbuf[5] = i/100;
      dispbuf[6] = i%100/10;
      dispbuf[7] = i%10;
    }
  }

  //实现数码管显示12345678
  dispbuf[0] = 1;
  dispbuf[1] = 2;
  dispbuf[2] = 3;
  dispbuf[3] = 4;
  dispbuf[4] = 5;
  dispbuf[5] = 6;
  dispbuf[6] = 7;
  dispbuf[7] = 8;
  //实现LED秒闪烁
  while(1)
  {
    if(gFlag_1000ms)
    {
      gFlag_1000ms = 0;
      LED_IO(i); //LED全亮或全灭
      i = ~i;
    }
    
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
  XBYTE[0XA000] = extdev_data;
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
  XBYTE[0X8000] = LEDdata;
  P2 &= 0X1F;//0b 000X XXXX //锁存数据
  EA = 1;//打开中断总使能
}

/////////////////////////////
void timer1_init(void)         //time1定时计数器配置为1ms的定时中断
{
	AUXR &= 0xBF;		//定时器时钟12T模式
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0x18;	  	//设置定时初始值
	TH1 = 0xFC;	  	//设置定时初始值
	TF1 = 0;	    	//清除TF1标志
	TR1 = 1;	    	//定时器1开始计时
  ET1 = 1;        //time1中断分允许
  EA = 1;         //中断总允许
}

void tm1_isr() interrupt 3
{
  gTimer_1ms++;
  if(gTimer_1ms == 60000)    gTimer_1ms=0;
  if(gTimer_1ms%10 == 0)    gFlag_10ms=1;
  if(gTimer_1ms%50 == 0)    gFlag_50ms=1;
  if(gTimer_1ms%100 == 0)   gFlag_100ms=1;
  if(gTimer_1ms%200 == 0)   gFlag_200ms=1;
  if(gTimer_1ms%250 == 0)   gFlag_250ms=1;
  if(gTimer_1ms%1000 == 0)  gFlag_1000ms=1;
  display_IO();
  
  
}
//////////////////////////////////////////////////
void display_IO(void)
{
  static unsigned char dispcom_IO = 0;

  P0 = 0xFF; //使段选全部输出为高，从而使所有数码管消隐
  P2 = (P2 & 0x1F)|0XE0;//0b 111X XXXX 使138译码输入CBA为7，Y7输出低电平，段选573的LE为高，锁存状态解除
  P2 &= 0x1F;//0b 000X XXXX //锁存数据
  P0 = 0;
  //位选
  P2 = (P2 & 0X1F)|0XC0;//0b 110X XXXX 使138译码输入CBA为6，Y6输出低电平，位选573的LE为高，锁存状态解除
  P0 = 1<<dispcom_IO;
  P2 &= 0X1F;//0b 000X XXXX //锁存数据
  //段选
  P0 = 0xFF;
  P2 = (P2 & 0X1F)|0XE0;//0b 111X XXXX 使138译码输入CBA为7，Y7输出低电平，段选573的LE为高，锁存状态解除
  P0 = ~disptab[dispbuf[dispcom_IO]];
  P2 &= 0x1F;//0b 000X XXXX //锁存数据

  dispcom_IO++;
  if (dispcom_IO >= 8) dispcom_IO = 0;
}
void display_MM(void)
{
  EA = 0;//关闭中断总使能
  XBYTE[0X8000] = 0;
  P2 &= 0X1F;//0b 000X XXXX //锁存数据
  EA = 1;//打开中断总使能
}

/////////////////////////////////
unsigned char BTN_KeyBoardScan(void)   //独立按键扫描程序，无有效按键按下时，返回0xFF;按键按下，返回对应序号；按键长按时，每过一段时间，返回对应序号+10；松手时，返回对应序号+20。
{
  static unsigned char KeyPressBTN = 0; //存储按键按压时长
  static unsigned char KeyFreeBTN = 0;  //按键松开标志
  static unsigned char KeyValueBTN = 0; //存储按键键值
  static unsigned char IntervalBTN = 55; //存储长按键时，键值返回的时间间隔，550ms
  unsigned char temp;
  P3 |= 0x0F;
  temp = (P3 & 0x0F);
  if(temp != 0x0F) //是否有键被按下
  {
    if(KeyPressBTN < 200) KeyPressBTN++;
    if(KeyPressBTN < 5)
    {
      return 0xFF;
    }
    else if(KeyPressBTN == 5)
    {
      KeyFreeBTN = 1;
      IntervalBTN = 55;
      switch(temp)
      {
        case 0x0E:KeyValueBTN = 7;break;
        case 0x0D:KeyValueBTN = 6;break;
        case 0x0B:KeyValueBTN = 5;break;
        case 0x07:KeyValueBTN = 4;break;
        //如果需要组合键，可根据键值增加情况
        default:KeyValueBTN = 0xFF; KeyFreeBTN = 0;IntervalBTN = 0;KeyPressBTN = 0;break;
      }
      return KeyValueBTN;
    }
    else //处理长按键
    {
      if(IntervalBTN)
      {
        IntervalBTN--;
        if(IntervalBTN == 0)
        {
          IntervalBTN = 55;
          return KeyValueBTN+10;
        }
      }
      else IntervalBTN = 55;
      return 0xFF;
    }
  }
  else //无键按下或已释放
  {
    KeyPressBTN = 0;
    if(KeyFreeBTN == 1)
    {
      KeyFreeBTN = 0;
      return KeyValueBTN+20;
    }
    return 0xFF;
  }
}
//////////////////////////////////////
unsigned char KBD_KeyBoardScan(void)  //矩阵按键扫描程序，无有效按键按下时，返回0xFF;按键按下，返回对应序号；按键长按时，过一段时间，返回对应序号+20；松手时，返回对应序号+40。（4-19)
{
  static unsigned char KeyPressKBD = 0; //存储按键按压时长
  static unsigned char KeyFreeKBD = 0;  //按键松开标志
  static unsigned char KeyValueKBD = 0; //存储按键键值
  static unsigned char IntervalKBD = 55; //存储长按键时，键值返回的时间间隔，550ms
  unsigned char S1;
  unsigned char S2;
  P3 = 0x0F;
  P42 = 0;
  P44 = 0;
  S1 = (P3 & 0x0F);
  if(S1 != 0x0F) //有键被按下
  {
    if(KeyPressKBD < 200) KeyPressKBD++;
    if(KeyPressKBD < 5)
    {
      return 0xFF;
    }
    else if(KeyPressKBD == 5)
    {
      KeyFreeKBD = 1;
      IntervalKBD = 55;
      S1 = (P3 & 0x0F);
      P3 = 0xF0;
      P42 = 1;
      P44 = 1;
      S2 = (P3 & 0xF0);
      if(P42) S2 |= 0x40;
      else S2 &= (~0x40);
      if(P44) S2 |= 0x80;
      else S2 &= (~0x80);
      S1 |= S2;

      switch(S1)
      {
        case 0x77:KeyValueKBD = 4;break;
        case 0x7B:KeyValueKBD = 5;break;
        case 0x7D:KeyValueKBD = 6;break;
        case 0x7E:KeyValueKBD = 7;break;
        case 0xB7:KeyValueKBD = 8;break;
        case 0xBB:KeyValueKBD = 9;break;
        case 0xBD:KeyValueKBD = 10;break;
        case 0xBE:KeyValueKBD = 11;break;
        case 0xD7:KeyValueKBD = 12;break;
        case 0xDB:KeyValueKBD = 13;break;
        case 0xDD:KeyValueKBD = 14;break;
        case 0xDE:KeyValueKBD = 15;break;
        case 0xE7:KeyValueKBD = 16;break;
        case 0xEB:KeyValueKBD = 17;break;
        case 0xED:KeyValueKBD = 18;break;
        case 0xEE:KeyValueKBD = 19;break;
        //如果需要组合键，可根据键值增加情况
        default:KeyValueKBD = 0xFF; KeyFreeKBD = 0;IntervalKBD = 0;KeyPressKBD = 0;break;
      }
      return KeyValueKBD;
    }
    else //处理长按键
    {
      if(IntervalKBD)
      {
        IntervalKBD--;
        if(IntervalKBD == 0)
        {
          IntervalKBD = 55;
          return KeyValueKBD+20;
        }
      }
      else IntervalKBD = 55;
      return 0xFF;
    }
  }
  else //无键按下或已释放
  {
    KeyPressKBD = 0;
    if(KeyFreeKBD == 1)
    {
      KeyFreeKBD = 0;
      return KeyValueKBD+40;
    }
    return 0xFF;
  }
}
