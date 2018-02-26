#ifndef __TRO_H
#define __TRO_H

#ifndef NULL
#define NULL (void*)0
#endif

typedef unsigned long uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;
typedef signed long int32_t;
typedef signed short int16_t;
typedef signed char int8_t;

//#define BUFFER_POS(X,Y) (Y*640+X)
#define BUFFER_POS(X,Y) ((Y)*640+(X))

#define RGB(R,G,B) (R << 12) | (G << 5) | (B << 0)

extern unsigned int sleep(unsigned int seconds);
extern unsigned int sleep_ms(unsigned int ms);
extern void draw_string(int x, int y,char* str, int color);

static volatile uint32_t *regs = (uint32_t*)0xA05F8000;
//static unsigned long* vram_l=(unsigned long*)0xa5000000;
volatile uint16_t* vram_s;//=(unsigned short*)0xa5000000;

extern void fillBox(int x, int y, int width, int height, int color);

//graphics
extern int memcpy(char *cdest, char *csrc, int n);
extern void memset(void *dest,int len);
extern void vid_waitvbl();
extern void v_circle(int x, int y, int radius, int r, int g, int b);
extern void initBuffers(void);

//input
extern uint16_t update_input(int btn);

#endif