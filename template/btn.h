#ifndef __BTN_H_
#define __BTN_H_

#define long_press_time 2000 //ms
#define scan_time 50 //ms

extern unsigned char Key_TempBTN,Key_DownBTN,Key_UpBTN,key_long_press;

unsigned char GetKeyBTN();

void btnStatus(void);

#endif
