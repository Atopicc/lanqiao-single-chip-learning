#include "main.h"
#include "seg.h"
#include "sonic.h"
#include "iic.h"
#include "btn.h"

unsigned char cnt_BTN; // 按键计数器
unsigned char LED_state = 0; // LED状态
unsigned char cnt_ADC = 0; // 电压计数器
unsigned char cnt_dist = 0; // 超声波计数器
unsigned char cnt_LED = 0; // LED计数器

unsigned char ADC_value;
unsigned int vol_disp;
unsigned int distance;
unsigned char DAC_value;

enum{
    Votage_ADC = 0,
    Set_param = 1,
    distace_range = 2
}interface;

typedef struct{
    unsigned char param_high;
    unsigned char param_low;
}param_struct;

param_struct param_temp;
param_struct param_set;
unsigned char *processing_param;
unsigned char *high_address;
unsigned char *low_address;

void timer1_init(void);
void keyprocess();
void interface_process();
void LED_process();

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
    cnt_BTN++;
    cnt_ADC++;
    cnt_dist++;
    cnt_LED++;
    display();
}

void main() // 主函数
{
    timer1_init();
    latch(5,0x00);
    interface = Votage_ADC;
    processing_param = &(param_temp.param_high);
    param_set.param_low = 5;
    param_set.param_high = 45;
    param_temp.param_low = 5;
    param_temp.param_high = 45;

    high_address = &(param_temp.param_high);
    low_address = &(param_temp.param_low);

    while (1)
    {
        if (cnt_BTN >= 50)
        {
            keyprocess();
        }
        if(cnt_ADC >= 150)
        {
            cnt_ADC = 0;
            ADC_value = PCF8591_ADC(3);
            vol_disp = (unsigned int)ADC_value * 5 / 0.256;
        }
        if(cnt_dist >= 250)
        {
            cnt_dist = 0;
            distance = GetDistence();
            //distace > 80, DAC = 255; distace < 20, DAC= 25; else DAC = (distance-20)*230/60;
            if(distance > 80) DAC_value = 255;
            else if(distance < 20) DAC_value = 51;
            else DAC_value = (distance-20)*3.4 + 51;
            if(!((vol_disp > param_set.param_low * 100) && (vol_disp < param_set.param_high * 100))) DAC_value = 0;
            PCF8591_DAC(DAC_value);
        }
        if(cnt_LED >= 100)
        {
            cnt_LED = 0;
            LED_process();
        }

        interface_process();
    }
}

void keyprocess() // 按键处理函数
{
    btnStatus();

    if (Key_DownBTN)
    {
        switch (Key_DownBTN)
        {
        case 4: //界面切换
            if(interface == Votage_ADC) interface = distace_range;
            else if(interface == distace_range) interface = Set_param;
            else
            {
                param_set.param_low = param_temp.param_low;
                param_set.param_high = param_temp.param_high;
                interface = Votage_ADC;
            }
            break;

        case 5: //设置参数
            if(interface == Set_param)
            {
                if(processing_param == high_address) processing_param = low_address;
                else processing_param = high_address;
            }
            break;

        case 6: //参数加 范围0-50 步进长度5 循环步进
            if(interface == Set_param)
            {
                if(*processing_param < 50) *processing_param += 5;
                else *processing_param = 5;
            }
            break;

        case 7: //参数减 范围0-50 步进长度5 循环步进
            if(interface == Set_param)
            {
                if(*processing_param > 5) *processing_param -= 5;
                else *processing_param = 50;
            }
            break;
        
        default:
            break;
        }
    }
}

void interface_process()
{
    if(interface == Votage_ADC)
    {
        dispbuf[0] = 25; //'U'
        dispbuf[1] = 16; //Blank
        dispbuf[2] = 16; //Blank
        dispbuf[3] = 16; //Blank
        dispbuf[4] = 16; //Blank
        dispbuf[5] = vol_disp/1000 + 32; //整数+小数点
        dispbuf[6] = (vol_disp%1000)/100;
        dispbuf[7] = (vol_disp%100)/10;
    }else if(interface == distace_range)
    {
        dispbuf[0] = 21; //'L'
        dispbuf[1] = 16; //Blank
        dispbuf[2] = 16; //Blank
        dispbuf[3] = 16; //Blank
        dispbuf[4] = 16; //Blank
        if((vol_disp > param_set.param_low * 100) && (vol_disp < param_set.param_high * 100))
        {
            if(distance >= 100) dispbuf[5] = distance/100;
            else dispbuf[5] = 16;
            if(distance >= 10) dispbuf[6] = (distance%100)/10;
            else dispbuf[6] = 16;
            dispbuf[7] = distance%10;
        }else
        {
            dispbuf[5] = 10; //'A'
            dispbuf[6] = 10; //'A'
            dispbuf[7] = 10; //'A'
        }
    }else
    {
        dispbuf[0] = 24; //'P'
        dispbuf[1] = 16; //Blank
        dispbuf[2] = 16; //Blank
        dispbuf[3] = param_temp.param_high/10 + 32; //整数+小数点
        dispbuf[4] = param_temp.param_high%10;
        dispbuf[5] = 16; //Blank
        dispbuf[6] = param_temp.param_low/10 + 32; //整数+小数点
        dispbuf[7] = param_temp.param_low%10;
    }
}

void LED_process()
{
    LED_state &= 0xF8;
    if(interface == Votage_ADC) LED_state |= 0x01;
    if(interface == distace_range) LED_state |= 0x02;
    if(interface == Set_param) LED_state |= 0x04;
    if((vol_disp > param_set.param_low * 100) && (vol_disp < param_set.param_high * 100))
    {
        if(LED_state & 0x80) LED_state &= 0x7F;
        else LED_state |= 0x80;
    }else LED_state &= 0x7F;

    latch(4,~LED_state);
}