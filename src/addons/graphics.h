#ifndef __GRAPHICS_H
#define __GRAPHICS_H

extern volatile uint16_t *backBuffer;

//graphics
extern void vid_waitvbl();
extern void v_circle(int x, int y, int radius, int r, int g, int b);
extern void initBuffers(void);

extern void fillBox(int x, int y, int width, int height, int color);

extern void bfont_draw_large_str(uint16_t *buffer, int width, char *str);
extern void draw_char24(int x, int y, int ch, int rgb);
extern void bfont_draw_mid_symbol(uint16_t *buffer, int chr);
extern void bfont_draw_large_letter(uint16_t *buffer, uint8_t c);
#endif