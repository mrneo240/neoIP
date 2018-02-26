/* This file is part of the libdream Dreamcast function library.
 * Please see libdream.c for further details.
 *
 * (c)2000 Dan Potter
 */

#include "maple.h"

/*

This module handles interfacing to the BIOS to get its "thin" font,
which is used in the BIOS menus. Among other useful properties (being
there without being loaded! =) this font includes Japanese characters.

Thanks to Marcus Comstedt for this information.

Ported from KallistiOS (Dreamcast OS) for libdream by Dan Potter

*/

int strlen(const char * str)
{
    const char *s;
    for (s = str; *s; ++s) {}
    return(s - str);
}

/*extern int strlen(const char *str);
asm (
"strlen:"
"    push ebp"
"    mov ebp, esp        ; setup the stack frame\n"
"    mov ecx, [ebp+8]\n"
"    xor eax, eax        ; loop counter\n"
"startLoop:\n"
"    xor dx, dx\n"
"    mov dl, byte [ecx+eax]\n"
"    inc eax\n"
"    cmp dl, 0x0 ; null byte\n"
"    jne startLoop\n"
"end:\n"
"    pop ebp\n"
"    ret\n"
);*/


/* A little assembly that grabs the font address */
extern void* get_font_address();

asm(
"_get_font_address:\n"
	"mov.l	syscall_b4,r0\n"
	"mov.l	@r0,r0\n"
	"jmp	@r0\n"
	"mov	#0,r1\n"
	
	".align 4\n"
"syscall_b4:\n"
"	.long	0x8c0000b4\n"
);


/* Given a character, find it in the BIOS font if possible */
void *bfont_find_char(int ch) {
	int index = -1;
	void *fa = get_font_address();
	
	/* 33-126 in ASCII are 1-94 in the font */
	if (ch >= 33 && ch <= 126)
		index = ch - 32;
	
	/* 160-255 in ASCII are 96-161 in the font */
	if (ch >= 160 && ch <= 255)
		index = ch - (160 - 96);
	
	/* Map anything else to a space */
	if (index == -1)
		index = 72 << 2;

	return fa + index*36;
}

/* Given a character, draw it into a buffer */
void bfont_draw(uint16 *buffer, int bufwidth, int c) {
	uint8 *ch = (uint8*)bfont_find_char(c);
	uint16 word;
	int x, y;

	for (y=0; y<24; ) {
		/* Do the first row */
		word = (((uint16)ch[0]) << 4) | ((ch[1] >> 4) & 0x0f);
		for (x=0; x<12; x++) {
			if (word & (0x0800 >> x))
				*buffer = 0xffff;
			buffer++;
		}
		buffer += bufwidth - 12;
		y++;
		
		/* Do the second row */
		word = ( (((uint16)ch[1]) << 8) & 0xf00) | ch[2];
		for (x=0; x<12; x++) {
			if (word & (0x0800 >> x))
				*buffer = 0xffff;
			buffer++;
		}
		buffer += bufwidth - 12;
		y++;
		
		ch += 3;
	}
}

void bfont_draw_str(uint16 *buffer, int width, char *str) {
	while (*str)
		bfont_draw(buffer += 12, width, *str++);
}

void bfont_draw_str_len(uint16 *buffer, char *str, int len) {
	int i=0;
	while (*str && i<len ){
		bfont_draw(buffer += 12, 640, *str++);
	i++;
	}
}












