#ifndef __UART_H_
#define __UART_H_

extern unsigned char Uart_Rev_Num;
extern unsigned char Uart_Rev_Data[20];
void Uart1_Int_T2(void);
void sendstr(unsigned char * straddr);
void sendbyte(unsigned char dat);


#endif