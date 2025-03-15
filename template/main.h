#ifndef __MAIN_H_
#define __MAIN_H_

#include <STC15F2K60S2.H>
#define latch(X,Y) {P0 = Y; P2 = ((P2 & 0x1F) | (X << 5)); P2 &= 0x1F;} 



#endif