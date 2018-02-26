#include "tro.h"
//http://paradise.untergrund.net/tmp/roinat/


void fillBox(int x, int y, int width, int height, int color)
{
	int i,j;
	for (i=x;i<x+width;i++){
		for (j=y;j<y+height;j++){
		*(vram_s+BUFFER_POS(i,j)) = color;
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
      *(vram_s+BUFFER_POS(x+xoff, y+yoff)) = RGB(r,g,b);
      *(vram_s+BUFFER_POS(x-xoff, y+yoff)) = RGB(r,g,b);
      *(vram_s+BUFFER_POS(x-xoff, y-yoff)) = RGB(r,g,b);
      *(vram_s+BUFFER_POS(x+xoff, y-yoff)) = RGB(r,g,b);
      
      *(vram_s+BUFFER_POS(x+yoff, y+xoff)) = RGB(r,g,b);
      *(vram_s+BUFFER_POS(x-yoff, y+xoff)) = RGB(r,g,b);
      *(vram_s+BUFFER_POS(x-yoff, y-xoff)) = RGB(r,g,b);
      *(vram_s+BUFFER_POS(x+yoff, y-xoff)) = RGB(r,g,b);
      
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
void vid_waitvbl() {
	volatile uint32_t *vbl = (uint32_t*)0xa05f810c;
	while (!(*vbl & 0x01ff))
		;
	while (*vbl & 0x01ff)
		;
}