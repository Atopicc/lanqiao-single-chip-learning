/*
  程序说明: IIC总线驱动程序
  软件环境: Keil uVision 4.10 
  硬件环境: CT107单片机综合实训平台 8051，12MHz
  日    期: 2011-8-9
*/

#include "stc15f2k60s2.h"
#include "intrins.h"
#include "iic.h"

#define DELAY_TIME 5

#define SlaveAddrW 0xA0
#define SlaveAddrR 0xA1


//总线引脚定义
sbit SDA = P2^1;  /* 数据线 */
sbit SCL = P2^0;  /* 时钟线 */

void IIC_Delay(unsigned char i)
{
    do{_nop_();}
    while(i--);        
}
//总线启动条件
void IIC_Start(void)
{
    SDA = 1;
    SCL = 1;
    IIC_Delay(DELAY_TIME);
    SDA = 0;
    IIC_Delay(DELAY_TIME);
    SCL = 0;	
}

//总线停止条件
void IIC_Stop(void)
{
    SDA = 0;
    SCL = 1;
    IIC_Delay(DELAY_TIME);
    SDA = 1;
    IIC_Delay(DELAY_TIME);
}

//发送应答
void IIC_SendAck(bit ackbit)
{
    SCL = 0;
    SDA = ackbit;  					// 0：应答，1：非应答
    IIC_Delay(DELAY_TIME);
    SCL = 1;
    IIC_Delay(DELAY_TIME);
    SCL = 0; 
    SDA = 1;
    IIC_Delay(DELAY_TIME);
}

//等待应答
bit IIC_WaitAck(void)
{
    bit ackbit;
	
    SCL  = 1;
    IIC_Delay(DELAY_TIME);
    ackbit = SDA;
    SCL = 0;
    IIC_Delay(DELAY_TIME);
    return ackbit;
}

//通过I2C总线发送数据
void IIC_SendByte(unsigned char byt)
{
    unsigned char i;

    for(i=0; i<8; i++)
    {
        SCL  = 0;
        IIC_Delay(DELAY_TIME);
        if(byt & 0x80) SDA  = 1;
        else SDA  = 0;
        IIC_Delay(DELAY_TIME);
        SCL = 1;
        byt <<= 1;
        IIC_Delay(DELAY_TIME);
    }
    SCL  = 0;  
}

//从I2C总线上接收数据
unsigned char IIC_RecByte(void)
{
    unsigned char i, da;
    for(i=0; i<8; i++)
    {   
    	SCL = 1;
	IIC_Delay(DELAY_TIME);
	da <<= 1;
	if(SDA) da |= 1;
	SCL = 0;
	IIC_Delay(DELAY_TIME);
    }
    return da;    
}

//单片机向EEPROM写一个字节的数据 
void write_EEPROM(unsigned char add,unsigned char EEPData)
{
    IIC_Start();                        //起始信号
    IIC_SendByte(SlaveAddrW);           //写控制字
    IIC_WaitAck();                      //应答
    IIC_SendByte(add);                  //发送要写数据的地址
    IIC_WaitAck();                      //应答
    IIC_SendByte(EEPData);              //发送要写的数据
    IIC_WaitAck();                      //应答
    IIC_Stop();                         //停止信号
}

//单片机向EEPROM读一个字节的数据
unsigned char read_EEPROM(unsigned char add) 
{
    unsigned char temp;
    IIC_Start();                        //起始信号
    IIC_SendByte(SlaveAddrW);           //伪写控制字
    IIC_WaitAck();                      //应答
    IIC_SendByte(add);                  //发送要读数据的地址
    IIC_WaitAck();                      //应答
    IIC_Start();                        //起始信号
    IIC_SendByte(SlaveAddrR);           //读控制字
    IIC_WaitAck();                      //应答
    temp = IIC_RecByte();               //读数据
    IIC_SendAck(1);                     //应答
    IIC_Stop();                         //停止信号
    return temp;
}

//单片机向8591写一个字节的数据,进行DA转换
void DAC_PCF8591(unsigned char DACData)
{
    IIC_Start();                        //起始信号
    IIC_SendByte(0x90);                 //写控制字
    IIC_WaitAck();                      //应答
    IIC_SendByte(0x40);                 //发送地址
    IIC_WaitAck();                      //应答
    IIC_SendByte(DACData);              //发送要写的数据
    IIC_WaitAck();                      //应答
    IIC_Stop();                         //停止信号
} 

//单片机AD转换，输入参数为通道号（0-3）
unsigned char ADC_PCF8591(unsigned char channel)
{
    unsigned char temp;
    IIC_Start();                        //起始信号
    IIC_SendByte(0x90);                 //伪写控制字
    IIC_WaitAck();                      //应答
    IIC_SendByte(channel);              //发送通道号
    IIC_WaitAck();                      //应答
    IIC_Start();                        //起始信号
    IIC_SendByte(0x91);                 //读控制字
    IIC_WaitAck();                      //应答
    temp = IIC_RecByte();               //读数据
    IIC_SendAck(1);                     //应答
    IIC_Stop();                         //停止信号
    return temp;
}
