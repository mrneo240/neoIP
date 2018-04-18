#include "neoIP.h"
#include "graphics.h"
//http://paradise.untergrund.net/tmp/roinat/


static volatile uint16_t* frontBuffer=0;
volatile uint16_t* backBuffer=0;
extern uint16_t *vrambase;

void fillBox(int x, int y, int width, int height, int color)
{
	int i,j;
	for (i=x; i<x+width; i++) {
		for (j=y; j<y+height; j++) {
			*(backBuffer+BUFFER_POS(i,j)) = color;
		}
	}
}

void v_circle(int x, int y, int radius, int r, int g, int b)
{
	int balance, xoff, yoff;

	xoff = 0;
	yoff = radius;
	balance=-radius;

	do {
		*(backBuffer+BUFFER_POS(x+xoff, y+yoff)) = RGB(r,g,b);
		*(backBuffer+BUFFER_POS(x-xoff, y+yoff)) = RGB(r,g,b);
		*(backBuffer+BUFFER_POS(x-xoff, y-yoff)) = RGB(r,g,b);
		*(backBuffer+BUFFER_POS(x+xoff, y-yoff)) = RGB(r,g,b);

		*(backBuffer+BUFFER_POS(x+yoff, y+xoff)) = RGB(r,g,b);
		*(backBuffer+BUFFER_POS(x-yoff, y+xoff)) = RGB(r,g,b);
		*(backBuffer+BUFFER_POS(x-yoff, y-xoff)) = RGB(r,g,b);
		*(backBuffer+BUFFER_POS(x+yoff, y-xoff)) = RGB(r,g,b);

		if ((balance+= xoff++ + xoff) >=0) {
			--yoff;
			balance-= yoff + yoff;
		}
	}
	while (xoff <= yoff);
}

/*
   Copied from: kernel/arch/dreamcast/hardware/video.c
   KallistiOS ##version##
   (c)2001 Anders Clerwall (scav)
   Parts (c)2000-2001 Dan Potter
  [This is the old KOS function by Dan.]
*/
void vid_waitvbl()
{
	volatile uint32_t *vbl = (uint32_t*)0xa05f810c;
	while (!(*vbl & 0x01ff))
		;
	while (*vbl & 0x01ff)
		;
}

/*-----------------------------------------------------------------------------*/
void vid_set_start(uint32_t base)
{
	/* Set vram base of current framebuffer */
	base &= 0x007FFFFF;
	videobase[0x14] = base;
}

/*-----------------------------------------------------------------------------*/
/* Clears the screen with a given color
    [This is the old KOS function by Dan.]
*/
void vid_clear(uint16_t color)
{
	sq_set16(backBuffer, color, (640 * 480) * 2);
}

/*-----------------------------------------------------------------------------*/
/* Clears all of video memory as quickly as possible
    [This is the old KOS function by Dan.]
*/
void vid_empty()
{
	/* We'll use the actual base address here since the vram_* pointers
	   can now move around */
	sq_clr((uint32_t *)0xa5000000, 8 * 1024 * 1024);
}

void initBuffers()
{
	frontBuffer=(uint16_t*)0xa5000000;
	backBuffer=(frontBuffer+(640*480));
	vrambase = backBuffer;

	vid_empty();
	vid_set_start((uint32_t*)backBuffer);
	vid_empty();
	vid_set_start((uint32_t*)frontBuffer);
}

void swapBuffers()
{
	uint16_t* temp;

	temp=backBuffer;
	backBuffer=frontBuffer;
	frontBuffer=temp;
	vrambase = backBuffer;
	vid_waitvbl();
	vid_set_start((uint32_t*)frontBuffer);
}

