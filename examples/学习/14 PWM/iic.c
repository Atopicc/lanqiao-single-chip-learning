/*
  ����˵��: IIC������������
  �������: Keil uVision 4.10 
  Ӳ������: CT107��Ƭ���ۺ�ʵѵƽ̨ 8051��12MHz
  ��    ��: 2011-8-9
*/

#include "stc15f2k60s2.h"
#include "intrins.h"
#include "iic.h"

#define DELAY_TIME 5

#define SlaveAddrW 0xA0
#define SlaveAddrR 0xA1


//�������Ŷ���
sbit SDA = P2^1;  /* ������ */
sbit SCL = P2^0;  /* ʱ���� */

void IIC_Delay(unsigned char i)
{
    do{_nop_();}
    while(i--);        
}
//������������
void IIC_Start(void)
{
    SDA = 1;
    SCL = 1;
    IIC_Delay(DELAY_TIME);
    SDA = 0;
    IIC_Delay(DELAY_TIME);
    SCL = 0;	
}

//����ֹͣ����
void IIC_Stop(void)
{
    SDA = 0;
    SCL = 1;
    IIC_Delay(DELAY_TIME);
    SDA = 1;
    IIC_Delay(DELAY_TIME);
}

//����Ӧ��
void IIC_SendAck(bit ackbit)
{
    SCL = 0;
    SDA = ackbit;  					// 0��Ӧ��1����Ӧ��
    IIC_Delay(DELAY_TIME);
    SCL = 1;
    IIC_Delay(DELAY_TIME);
    SCL = 0; 
    SDA = 1;
    IIC_Delay(DELAY_TIME);
}

//�ȴ�Ӧ��
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

//ͨ��I2C���߷�������
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

//��I2C�����Ͻ�������
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

//��Ƭ����EEPROMдһ���ֽڵ����� 
void write_EEPROM(unsigned char add,unsigned char EEPData)
{
    IIC_Start();                        //��ʼ�ź�
    IIC_SendByte(SlaveAddrW);           //д������
    IIC_WaitAck();                      //Ӧ��
    IIC_SendByte(add);                  //����Ҫд���ݵĵ�ַ
    IIC_WaitAck();                      //Ӧ��
    IIC_SendByte(EEPData);              //����Ҫд������
    IIC_WaitAck();                      //Ӧ��
    IIC_Stop();                         //ֹͣ�ź�
}

//��Ƭ����EEPROM��һ���ֽڵ�����
unsigned char read_EEPROM(unsigned char add) 
{
    unsigned char temp;
    IIC_Start();                        //��ʼ�ź�
    IIC_SendByte(SlaveAddrW);           //αд������
    IIC_WaitAck();                      //Ӧ��
    IIC_SendByte(add);                  //����Ҫ�����ݵĵ�ַ
    IIC_WaitAck();                      //Ӧ��
    IIC_Start();                        //��ʼ�ź�
    IIC_SendByte(SlaveAddrR);           //��������
    IIC_WaitAck();                      //Ӧ��
    temp = IIC_RecByte();               //������
    IIC_SendAck(1);                     //Ӧ��
    IIC_Stop();                         //ֹͣ�ź�
    return temp;
}

//��Ƭ����8591дһ���ֽڵ�����,����DAת��
void DAC_PCF8591(unsigned char DACData)
{
    IIC_Start();                        //��ʼ�ź�
    IIC_SendByte(0x90);                 //д������
    IIC_WaitAck();                      //Ӧ��
    IIC_SendByte(0x40);                 //���͵�ַ
    IIC_WaitAck();                      //Ӧ��
    IIC_SendByte(DACData);              //����Ҫд������
    IIC_WaitAck();                      //Ӧ��
    IIC_Stop();                         //ֹͣ�ź�
} 

//��Ƭ��ADת�����������Ϊͨ���ţ�0-3��
unsigned char ADC_PCF8591(unsigned char channel)
{
    unsigned char temp;
    IIC_Start();                        //��ʼ�ź�
    IIC_SendByte(0x90);                 //αд������
    IIC_WaitAck();                      //Ӧ��
    IIC_SendByte(channel);              //����ͨ����
    IIC_WaitAck();                      //Ӧ��
    IIC_Start();                        //��ʼ�ź�
    IIC_SendByte(0x91);                 //��������
    IIC_WaitAck();                      //Ӧ��
    temp = IIC_RecByte();               //������
    IIC_SendAck(1);                     //Ӧ��
    IIC_Stop();                         //ֹͣ�ź�
    return temp;
}
