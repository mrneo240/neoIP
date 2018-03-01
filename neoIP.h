#ifndef __NEOIP_H
#define __NEOIP_H

#ifndef NULL
#define NULL (void*)0
#endif

typedef unsigned long uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;
typedef signed long int32_t;
typedef signed short int16_t;
typedef signed char int8_t;


#include "libc.h"

#define BUFFER_POS(X,Y) ((Y)*640+(X))
#define RGB(R,G,B) (R << 12) | (G << 5) | (B << 0)

extern unsigned int sleep(unsigned int seconds);
extern unsigned int sleep_ms(unsigned int ms);
extern void draw_string(int x, int y,char* str, int color);

//static volatile uint32_t *regs = (uint32_t*)0xA05F8000;
extern uint32_t *videobase; //changed to use the declaration in video.s

//input
extern uint16_t update_input(int btn);

#endif