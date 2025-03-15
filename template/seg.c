#include <STC15F2K60S2.H>
#include "main.h"

//码表
unsigned char code disptab[]={                       //标准字库(共阴)
//   0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F  (0-15)
    0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,
//black  -     H    J    K    L    N    o   P    U     t    G    Q    r   M    y  (16-31)
    0x00,0x40,0x76,0x1E,0x70,0x38,0x37,0x5C,0x73,0x3E,0x78,0x3d,0x67,0x50,0x37,0x6e,
   //0.   1.   2.  3.    4.    5.  6.   7.   8.    9.   -1                        (32-42)
    0xBF,0x86,0xDB,0xCF,0xE6,0xED,0xFD,0x87,0xFF,0xEF,0x46};

unsigned char dispbuf[8] = {16,16,16,16,16,16,16,16};
		
void display(void)
{
	static unsigned char i = 0;
	latch(6,0x00);
	latch(6,1<<i);
	P0 = 0xFF;
	latch(7,~(disptab[dispbuf[i]]));
	i = (i+1)%8;
}

//清除显示
void clear(void)
{
	unsigned char i;
	for(i=0;i<8;i++)
		dispbuf[i] = 16;
	display();
}
