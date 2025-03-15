#include "main.h"
#include "seg.h"
#include "btn.h"

unsigned char cnt_BTN; // 按键计数器
unsigned char LED_Data = 0x00;

void timer1_init(void); // 初始化定时器1
void keyprocess(void);

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
    display();
}

void main() // 主函数
{
    latch(4,~LED_Data);
    latch(5,0x00);
    timer1_init();
    while (1)
    {
        if (cnt_BTN >= 50)
        {
            cnt_BTN = 0;
            keyprocess();
            // 按键任务
        }
    }
}

void keyprocess() // 按键处理函数
{
    btnStatus();

    if (Key_DownBTN)
    {
        dispbuf[0] = Key_DownBTN / 10;
        dispbuf[1] = Key_DownBTN % 10;
    }

    if (Key_UpBTN)
    {
        dispbuf[3] = Key_UpBTN / 10;
        dispbuf[4] = Key_UpBTN % 10;
    }

    if(key_long_press)
    {
        dispbuf[6] = key_long_press / 10;
        dispbuf[7] = key_long_press % 10;
    }
}
