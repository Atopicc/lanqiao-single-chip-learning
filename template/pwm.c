#include <STC15F2K60S2.H>

unsigned char pwm(unsigned char duty)
{
	unsigned int t1;
	t1 = (TH1<<8)|TL1;
	if(t1 >= (0xFC18+(duty*10))) return 0;
	else return 1;
}
