#include <STC15F2K60S2.H>

unsigned char Uart_Rev_Data[20];
unsigned char Uart_Rev_Num; // *需测试是否全局变量

void Uart1_Int_T2(void)
{
  SCON = 0x50;
  AUXR |= 0x01;
  AUXR |= 0x04;
  T2L = 0xC7;
  T2H = 0xFE;
  AUXR |= 0x10;
  ES = 1; // 串口中断允许位
  EA = 1; // 中断总允许位
}

void sendbyte(unsigned char dat)
{
  SBUF = dat;
  while (TI == 0)
    ;
  TI = 0;
}

void sendstr(unsigned char *straddr)
{
  while (*straddr != '\0')
    sendbyte(*straddr++);
}

void Uart(void) interrupt 4
{
  if (RI)
  {
    RI = 0;                             // 清除RI位
    Uart_Rev_Data[Uart_Rev_Num] = SBUF; // 存储接收到的数据
    Uart_Rev_Num++;
    if (Uart_Rev_Num >= 20)
      Uart_Rev_Num = 0;
  }
}
