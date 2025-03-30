#include "main.h"
#include "seg.h"
#include "kbd.h"
#include "onewire.h"

unsigned char cnt_KBD; // 按键计数器
unsigned char LED_Data = 0x00;
unsigned int cnt_display;
unsigned char cnt_temp;
unsigned char cnt_LED;
unsigned int tempData;
int AdjustData = 0;
int ParamData = 26;
unsigned char TriggerMod = 1;

enum{
    temprature = 0,
    adjust,
    param
}interface;

void timer1_init(void); // 初始化定时器1
void keyprocess(void);
void interfaceProcess(void);
void LED_Process(void);
unsigned char isNeg(int dat);

void timer1_init(void) // time1定时计数器配置为100ms的定时中断
{
    AUXR &= 0xBF; // 定时器时钟12T模式
    TMOD &= 0x0F; // 设置定时器模式
    TL1 = 0x18;   // 设置定时初始值
    TH1 = 0xFC;   // 设置定时初始值
    TF1 = 0;      // 清除TF1标志
    TR1 = 1;      // 定时器1开始计时
    ET1 = 1;      // time1中断分允许
    EA = 1;       // 中断总允许
}

void tm1_isr() interrupt 3 // 定时器1中断服务程序
{
    cnt_KBD++;
    cnt_temp++;
    cnt_LED++;
    display();
    latch(4,~LED_Data);
}

void main() // 主函数
{
	interface = temprature;
    latch(5,0x00);
    timer1_init();
    while (1)
    {
		interfaceProcess();
		LED_Process();
        if(cnt_KBD >= 50)
        {
            cnt_KBD = 0;
            keyprocess();
        }
        if(cnt_temp >= 200)
        {
            cnt_temp = 0;
            tempData = GetTemputerature()*100;
            tempData += AdjustData*100;
        }
    }
}

void keyprocess() // 按键处理函数
{
    kbdProcess();

    if(Key_Down)
    {
        if(Key_Down == 4)
        {
            interface = (interface+1)%3;
        }

        if(Key_Down == 5)
        {
            if(TriggerMod) TriggerMod = 0;
            else TriggerMod = 1;
        }

        if(Key_Down == 8)
        {
            if(interface == adjust)
            {
                if(AdjustData > -99) AdjustData -= 1;
            }
            if(interface == param)
            {
                if(ParamData > -99) ParamData -= 1;
            }
        }

        if(Key_Down == 9)
        {
            if(interface == adjust)
            {
                if(AdjustData < 99) AdjustData += 1;
            }
            if(interface == param)
            {
                if(ParamData < 99) ParamData += 1;
            }
        }
    }
}

void interfaceProcess()
{
    if(interface == temprature)
    {
        dispbuf[0] = 12; // 'C'

        dispbuf[1] = 16;
        dispbuf[2] = 16;
        dispbuf[3] = 16;
        dispbuf[4] = 16;

        if(tempData >= 1000) dispbuf[5] = tempData/1000;
        else dispbuf[5] = 16;
        dispbuf[6] = tempData%1000/100 + 32;
        dispbuf[7] = tempData%100/10;
    }
    if(interface == adjust)
	{
        dispbuf[0] = 14; // 'E'

        dispbuf[1] = 16;
        dispbuf[2] = 16;
        dispbuf[3] = 16;
        dispbuf[4] = 16;

        if(isNeg(AdjustData))
        {
            int adjust_temp;
            adjust_temp = -AdjustData;
            if(AdjustData <= -10)
            {
                dispbuf[5] = 17;
                dispbuf[6] = adjust_temp/10;
                dispbuf[7] = adjust_temp%10;
            }else
            {
                dispbuf[5] = 16;
                dispbuf[6] = 17;
                dispbuf[7] = adjust_temp%10;
            }
        }else
        {
            dispbuf[5] = 16;
            if(AdjustData >= 10) dispbuf[6] = AdjustData/10;
            else dispbuf[6] = 16;
            dispbuf[7] = AdjustData%10;
        }
    }
    if(interface == param)
	{
        dispbuf[0] = 18; // 'HH'

        dispbuf[1] = 16;
        dispbuf[2] = 16;
        dispbuf[3] = 16;
        dispbuf[4] = 16;

        if(isNeg(ParamData))
        {
            int param_temp;
            param_temp = -ParamData;
            if(ParamData <= -10)
            {
                dispbuf[5] = 17;
                dispbuf[6] = param_temp/10;
                dispbuf[7] = param_temp%10;
            }else
            {
                dispbuf[5] = 16;
                dispbuf[6] = 17;
                dispbuf[7] = param_temp%10;
            }
        }else
        {
            dispbuf[5] = 16;
            if(ParamData >= 10) dispbuf[6] = ParamData/10;
            else dispbuf[6] = 16;
            dispbuf[7] = ParamData%10;
        }
    }
}

void LED_Process()
{
    if(interface == temprature) LED_Data |= 0x01;
    else LED_Data &= 0xFE;

    if(interface == adjust) LED_Data |= 0x02;
    else LED_Data &= 0xFD;

    if(interface == param) LED_Data |= 0x04;
    else LED_Data &= 0xFB;

    if(TriggerMod)
    {
        LED_Data |= 0x08;
        LED_Data &= 0xEF;
    }else 
    {
        LED_Data |= 0x10;
        LED_Data &= 0xF7;
    }

    if(TriggerMod && (tempData > (ParamData*100)))
        LED_Data |= 0x80;
    else if((!TriggerMod) && (tempData < (ParamData*100)))
        LED_Data |= 0x80;
    else LED_Data &= 0x7F;
}

unsigned char isNeg(int dat)
{
    return (dat&0x8000)>>15;
}