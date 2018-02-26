#include "maple.h"
#include "tro.h"
#include "patches.h"

uint16_t update_input(int btn)
{
	struct mapledev *dev;
	int i;

	if(dev = check_pads()) {
		for(i=0; i<4; i++) {
			switch(dev[i].func) {
			case MAPLE_FUNC_CONTROLLER:
				if(!(dev[i].cond.controller.buttons & btn))
					return 0;
			}
		}
	}
	return 1;
}


//------------------------------------
// Main Stuff
//------------------------------------
extern void start2(void);
extern void fixbin(int choice);
extern void bfont_draw_large_str(uint16_t *buffer, int width, char *str);
extern void draw_char24(int x, int y, int ch, int rgb);
extern void bfont_draw_mid_symbol(uint16_t *buffer, int chr);
extern void bfont_draw_large_letter(uint16_t *buffer, uint8_t c);

void main()
{
	extern int rgb;
	uint16_t controls = 0;

	extern struct patch_data PATCHES;

	PATCHES.patch_num = 0;

	clrscr(0);
	init_video(check_cable(),1);
	maple_init();
	initBuffers();

	//Copy Game Title from IP.BIN to temp area
	memset(0xACDAFF00,0x2f);
	memcpy(0xACDAFF00,0x8C008080,0x2e);

	controls = 1;
	int flashing = 0;
	int selection = -1;

	struct a_menu *my_menu = new_menu ("Cheats and Patches");
	add_option(my_menu, PATCHES.patches[0].text);
	add_option(my_menu, PATCHES.patches[1].text);
	add_option(my_menu, PATCHES.patches[2].text);
	add_option(my_menu, PATCHES.patches[3].text);


	do {
		controls = update_input(CONT_START);
		vid_clear(0xD7FF);

		//Draw Boxes to establish Menu Background
		fillBox(600,40+16,18,400,RGB(0,0,0));
		fillBox(40+18,440,560,16,RGB(0,0,0));
		fillBox(40,40,560,400,0xE71C);
		//Draw line under title
		fillBox(40,70,560,2,0x0000);

		//Draw Game Title
		draw_string(52,52,(char*)0xACDAFF00,0x0000);

	
		//Draw Text
		rgb = 0x8410;
		bfont_draw_large_str(vram_s+BUFFER_POS(52,400),640,"PRESS");
		bfont_draw_large_str(vram_s+BUFFER_POS(276,400),640,"TO BOOT");
		rgb = 0xffff;

		//Draw Symbols

		//Start button
		rgb = RGB(0,156,210);
		bfont_draw_mid_symbol(vram_s+BUFFER_POS(224,404),518400+(20*72));
		rgb= 0xFFFF;

		selection = display_menu (my_menu, 64, 74, 0);
		switch(selection) {
		case 1:
			break;
		case 2:
			break;
		case 3:
			break;
		case 4:
			break;
		case 0:
		default:
			break;
		}
		sleep_ms(60);
		
		swapBuffers();
	}
	while(controls);

	vid_clear(0);


	/*controls=1;
	do{
		controls = update_input(CONT_START);
		if(!update_input(CONT_X)){
			char *str ="RUNTIME PATCHING  ";
			char *s;
			int i=0;
			for (s = str; *s; ++s) {
				((char*)(0x8c010000+0xBF8A0))[i]=str[i++];
			}
			break;
		}
	} while(controls);*/
	start2();
}

