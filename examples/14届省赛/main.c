#include "main.h"
#include "seg.h"
#include "ds1302.h"
#include "iic.h"
#include "frequence.h"
#include "kbd.h"
#include "onewire.h"

#define dark 50 

unsigned char cnt_KBD; // 按键计数器
unsigned char LED_Data = 0x00;
unsigned int cnt_RTC;
unsigned int cnt_freq1,cnt_freq2;
unsigned char cnt_ADC;
unsigned int cnt_display;
unsigned char cnt_temp;
unsigned char cnt_LED;

typedef struct
{
    unsigned int freq;
    unsigned char humi;
    unsigned char is_valid;
} humidity_t;

humidity_t humidity_data = {0,0,1};

typedef struct
{
    unsigned char triigerTimes;
    unsigned char hour;
    unsigned char minute;
} param_t;
param_t param_data = {0,0,0};

typedef struct
{
    unsigned char max;
    float average;
} param_data_t;
param_data_t triggered_tempra = {0,0};
param_data_t triggered_humi_data = {0,0};
unsigned char last_temp_data = 0,last_humi_data = 0,LED_Flag = 0,LED_Flag2 = 1;

unsigned char temp_data;
unsigned char light_data = 0xFF;
unsigned char last_light_data = 0xFF;

bit is_trigger;

unsigned char tempParam = 30;

enum{
    time = 0,
    recall,
    param,
    status
}interface;
unsigned char origin_interface;

enum{
    temprature = 0,
    humidity,
    time_recall
}recall_item;

void timer1_init(void); // 初始化定时器1
void keyprocess(void);
void interfaceProcess(void);
void LED_Process(void);

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
    cnt_RTC++;
    cnt_freq1++;
    cnt_freq2++;
    cnt_ADC++;
    cnt_temp++;
    cnt_LED++;
    if(is_trigger)
    {
        cnt_display++;
    }
    if(cnt_freq1 == 1) Timer0_Init_MeasureFreq();
    if(cnt_freq1 >= 501)
    {
        cnt_freq1 = 0;
        cnt_freq2 = 1;
        humidity_data.freq = GetFrequence();
    }
    display();
    latch(4,~LED_Data);
}

void main() // 主函数
{
	interface = time;
	PCF8591_ADC(1);
    latch(5,0x00);
    SetRTC();
    timer1_init();
    while (1)
    {
		interfaceProcess();
        
        if (cnt_KBD >= 50)
        {
            cnt_KBD = 0;
            keyprocess();
        }

        if(cnt_LED >= 100)
        {
            cnt_LED = 0;
            LED_Process();
        }

        if(cnt_temp >= 200)
        {
            temp_data = GetTemputerature();
            cnt_temp = 0;
        }

        if(cnt_freq2)
        {
            cnt_freq2 = 0;
            if(humidity_data.freq>2000 || humidity_data.freq<200)
                humidity_data.is_valid = 0;
            else
            {
                humidity_data.is_valid = 1;
                humidity_data.humi = 10+(humidity_data.freq-200)*0.0444444;
            }
        }

        if(cnt_ADC >= 150)
        {
            cnt_ADC = 0;
            last_light_data = light_data;
            light_data = PCF8591_ADC(1);
            if((light_data < dark) && (!(is_trigger)) && (last_light_data >= dark)) //触发
            {
                origin_interface = interface;
                interface = status;
                is_trigger = 1;
                if(humidity_data.is_valid)
                {
                    unsigned int sum1 = 0,sum2 = 0;
                    LED_Flag2 = 1;
                    if(param_data.triigerTimes>0 && last_humi_data < humidity_data.humi && last_temp_data < temp_data)
                        LED_Flag = 1;
                    else LED_Flag = 0;
                    last_temp_data = temp_data;
                    last_humi_data = humidity_data.humi;
                    param_data.triigerTimes++;
                    param_data.hour = ReadRTC_Data[4];
                    param_data.minute = ReadRTC_Data[5];
                    if(triggered_humi_data.max < humidity_data.humi) triggered_humi_data.max = humidity_data.humi;
                    sum1 = triggered_humi_data.average*(param_data.triigerTimes-1) + humidity_data.humi;
                    triggered_humi_data.average = sum1/param_data.triigerTimes;
                    if(triggered_tempra.max < temp_data) triggered_tempra.max = temp_data;
                    sum2 = triggered_tempra.average*(param_data.triigerTimes-1) + temp_data;
                    triggered_tempra.average = sum2/param_data.triigerTimes;
                }else
                {
                    LED_Flag2 = 0;
                }
            }
        }

        if(cnt_display >= 3000)
        {
            is_trigger = 0;
            interface = origin_interface;
            cnt_display = 0;
        }

        if(cnt_RTC >= 500)
        {
            cnt_RTC = 0;
            ReadRTC();
        }
    }
}

void keyprocess() // 按键处理函数
{
    kbdProcess();

    if (Key_Down == 4) //切换界面
    {
        interface = (interface+1)%3;
        if(interface == recall)
        {
            recall_item = temprature;
        }
    }
    if(Key_Down == 5 && interface == recall)
    {
        recall_item = (recall_item+1)%3;
    }
    if(Key_Down == 8 && interface == param)
    {
        if(tempParam < 99)
            tempParam += 1;
    }
    if(Key_Down == 9 && interface == param)
    {
        if(tempParam > 0)
            tempParam -= 1;
    }
    if(Key_long_press == 9 && interface == recall && recall_item == time_recall)
    {
        param_data.triigerTimes = 0;
        param_data.hour = 0;
        param_data.minute = 0;
        triggered_tempra.average = 0;
        triggered_tempra.max = 0;
        triggered_humi_data.average = 0;
        triggered_humi_data.max = 0;
    }
}

void interfaceProcess()
{
    if(interface == time)
    {
        dispbuf[0] = ReadRTC_Data[4]>>4;
        dispbuf[1] = ReadRTC_Data[4]&0x0F;
        dispbuf[2] = 17;
        dispbuf[3] = ReadRTC_Data[5]>>4;
        dispbuf[4] = ReadRTC_Data[5]&0x0F;
        dispbuf[5] = 17;
        dispbuf[6] = ReadRTC_Data[6]>>4;
        dispbuf[7] = ReadRTC_Data[6]&0x0F;
    }
    if(interface == recall)
    {
        if(recall_item == temprature)
        {
            dispbuf[0] = 12; //"C"
            dispbuf[1] = 16; //blank
            dispbuf[2] = triggered_tempra.max/10;
            dispbuf[3] = triggered_tempra.max%10;
            dispbuf[4] = 17; //"-"
            dispbuf[5] = triggered_tempra.average/10;
            dispbuf[6] = ((unsigned char)triggered_tempra.average%10) + 32;
            dispbuf[7] = (unsigned char)(triggered_tempra.average*10)%10;
        }
        if(recall_item == humidity)
        {
            dispbuf[0] = 18; //"H"
            dispbuf[1] = 16; //blank
            dispbuf[2] = triggered_humi_data.max/10;
            dispbuf[3] = triggered_humi_data.max%10;
            dispbuf[4] = 17; //"-"
            dispbuf[5] = triggered_humi_data.average/10;
            dispbuf[6] = ((unsigned char)triggered_humi_data.average%10) + 32;
            dispbuf[7] = (unsigned char)(triggered_humi_data.average*10)%10;
        }
        if(recall_item == time_recall)
        {
            dispbuf[0] = 15; //"F"
            dispbuf[1] = param_data.triigerTimes/10;
            dispbuf[2] = param_data.triigerTimes%10;
            dispbuf[3] = param_data.hour/10;
            dispbuf[4] = param_data.hour%10;
            dispbuf[5] = 17; //":"
            dispbuf[6] = param_data.minute/10;
            dispbuf[7] = param_data.minute%10;
        }
    }
    if(interface == param)
    {
        dispbuf[0] = 24; //"P"
        dispbuf[1] = 16; //blank
        dispbuf[2] = 16; //blank
        dispbuf[3] = 16; //blank
        dispbuf[4] = 16; //blank
        dispbuf[5] = 16; //blank
        dispbuf[6] = tempParam/10;
        dispbuf[7] = tempParam%10;
    }
    if(interface == status)
    {
        dispbuf[0] = 14; //"E"
        dispbuf[1] = 16; //blank
        dispbuf[2] = 16; //blank
        dispbuf[3] = temp_data/10;
        dispbuf[4] = temp_data%10;
        dispbuf[5] = 17; //"-"
        if(LED_Flag2)
        {
            dispbuf[6] = humidity_data.humi/10;
            dispbuf[7] = humidity_data.humi%10;
        }else
        {
            dispbuf[6] = 10; //"A"
            dispbuf[7] = 10; //"A"
        }
    }
}

void LED_Process()
{
    static unsigned char temp_flag = 0;
    if(interface == time) LED_Data |= 0x01;
    else LED_Data &= 0xFE;
    if(interface == recall) LED_Data |= 0x02;
    else LED_Data &= 0xFD;
    if(interface == param) LED_Data |= 0x04;
    else LED_Data &= 0xFB;

    if(temp_data > tempParam)
    {
        if(temp_flag)
        {
            LED_Data |= 0x08;
            temp_flag = 0;
        }else
        {
            LED_Data &= 0xF7;
            temp_flag = 1;
        }
    }else LED_Data &= 0xF7;
    if(LED_Flag2) LED_Data &= 0xEF;
    else LED_Data |= 0x10;

    if(LED_Flag)
    {
        LED_Data |= 0x20;
    }else LED_Data &= 0xDF;
}
