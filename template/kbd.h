#ifndef __KBD_H_
#define __KBD_H_

extern unsigned char Key_Temp,Key_Down,Key_Up;
extern unsigned char Key_Old;

unsigned char Key_Read();

void kbdProcess(void);
	
//extern unsigned char trg;
//extern unsigned char count;

//unsigned char KBD(void);
//void KBD_key_read(void);

#endif