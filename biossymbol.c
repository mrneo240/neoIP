/* This file is part of the libdream Dreamcast function library.
 * Please see libdream.c for further details.
 *
 * (c)2000 Dan Potter
 */

#include "tro.h"

/*

This module handles interfacing to the BIOS to get its "thin" font,
which is used in the BIOS menus. Among other useful properties (being
there without being loaded! =) this font includes Japanese characters.

Thanks to Marcus Comstedt for this information.

Ported from KallistiOS (Dreamcast OS) for libdream by Dan Potter

*/
int rgb;

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
/*
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
*/

/* Given a character, find it in the BIOS font if possible */
void *bfont_find_char(uint32_t ch) {
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
void bfont_draw(uint16_t *buffer, uint32_t bufwidth, int c) {
	uint8_t *ch = (uint8_t*)bfont_find_char(c);
	uint16_t word;
	int x, y;

	for (y=0; y<24; ) {
		/* Do the first row */
		word = (((uint16_t)ch[0]) << 4) | ((ch[1] >> 4) & 0x0f);
		for (x=0; x<12; x++) {
			if (word & (0x0800 >> x))
				*buffer = 0xffff;
			buffer++;
		}
		buffer += bufwidth - 12;
		y++;
		
		/* Do the second row */
		word = ( (((uint16_t)ch[1]) << 8) & 0xf00) | ch[2];
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

void bfont_draw_str(uint16_t *buffer, int width, char *str) {
	while (*str)
		bfont_draw(buffer += 12, width, *str++);
}

char* toUpper(char* string)
{
	char* p;
    for(p=string; *p != '\0'; p++)
    {
        if(*p >= 'a' && *p <= 'z')  //Only if it's a lower letter
          *p -= 32;
    }
    return string;
}

char upper(char p)
{
	if(p >= 'a' && p <= 'z')  //Only if it's a lower letter
          p -= 32;
	return p;
}

/* Given a character, find it in the BIOS font if possible */
void *bfont_find_symbol(int ch) {
	int index = -1;
	void *fa = get_font_address();
	
	/* 65-90 in ASCII are 95-120 in the symbol table */
	if (ch >= 'A' && ch <= 'Z')
		index = ch + 30;
	
	/* 48-57 in ASCII are ????? in the symbol table */
	if (ch >= '0' && ch <= '9')
		index = ch + 37;

	return fa + 520112+ (index*128);
}

#define GETMASK(index, size) (((1 << (size)) - 1) << (index))
#define READFROM(data, index, size) (((data) & GETMASK((index), (size))) >> (index))
void bfont_draw_large_letter(uint16_t *buffer, uint8_t c){
	int bufwidth = 640;
	uint8_t *ch = (uint8_t*)bfont_find_symbol(c);
	uint16_t word;
	buffer+=bufwidth;
	int x,y,z,j;
	x=0;y=0;
	for(z=0;z<128;z++){
		if(x==32){x=0;y++;buffer+=bufwidth-32;}
		for(j=0;j<8;j++){
			if(READFROM(ch[z],7-j,1)){
				*buffer = (rgb !=0 ? rgb :0xffff);
			}
			buffer++;
			x++;
		}
	}
}

static int bfont_fgcolor = 0xFFFF;
void bfont_draw_mid_symbol(uint16_t *buffer, int chr){
	uint8_t   *ch = get_font_address()+chr;
	int bufwidth = 640;
    uint16_t  word;
    int x, y;

	for(y = 0; y < 24;) {
		/* Do the first row */
		word = (((uint16_t)ch[0]) << 4) | ((ch[1] >> 4) & 0x0f);

		for(x = 0; x < 12; x++) {
			if(word & (0x0800 >> x))
				*buffer = (rgb !=0 ? rgb :0xffff);//bfont_fgcolor;
			buffer++;
		}
		word = ((((uint16_t)ch[1]) << 8) & 0xf00) | ch[2];
		for(x = 0; x < 12; x++) {
			if(word & (0x0800 >> x))
				*buffer = (rgb !=0 ? rgb :0xffff);//bfont_fgcolor;
			buffer++;
		}
		buffer += bufwidth - 24;
		y++;

		ch += 3;
	}
}

void bfont_draw_symbol(uint16_t *buffer, char c){
	int bufwidth = 640;
	uint8_t *ch = (uint8_t*)bfont_find_symbol(c);
	uint16_t word;
	int x,y,z,j;
	x=0;y=0;
	for(z=0;z<128;z++){
		if(x==32){x=0;y++;buffer+=bufwidth-32;}
		for(j=0;j<8;j++){
			if(READFROM(ch[z],7-j,1)){
				*buffer = 0xffff;
			}
			buffer++;
			x++;
		}
	}
}

void bfont_draw_large_str(uint16_t *buffer, int width, char *str) {
	for (str; *str != '\0'; str++){
		if(*str == ' '){buffer+=32;str++;}
		bfont_draw_large_letter(buffer, upper(*str));
		buffer += 32;
	}
	//while (*str)
	//	bfont_draw_large_letter(buffer += 32, *str++);
}







