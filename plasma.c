#include "maple.h"
#include "tro.h"

uint16 update_input(int btn)
{
  struct mapledev *dev;
  int i;

	if(dev = check_pads()){
		for(i=0; i<4; i++){
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
extern void bfont_draw_large_str(uint16 *buffer, int width, char *str);
extern void draw_char24(int x, int y, int ch, int rgb);
extern void bfont_draw_mid_symbol(uint16 *buffer, int chr);
extern void bfont_draw_large_letter(uint16 *buffer, uint8 c);

void main()
{
	
	extern int rgb;
	uint16 controls = 0;
	
	clrscr(0);
	init_video(check_cable(),1);
	maple_init();

	//draw_string(50,50,"Patching...",0xffff);
	//draw_string(400,50,"BP 0x8c00c7e0",0xffff);
	

	//draw_string(50,120,(char*)0xac010000+0x3FF85,0xffff);
	
	//draw_string(50,440,"Press X to patch",0xffff);
	//draw_string(50,400,"Press B to skip",0xffff);
	controls = 1;
	int flashing = 0;
	do{
		controls = update_input(CONT_START);
		clrscr(0);	
		//bfont_draw_large_str(vram_s+BUFFER_POS(0,0),640,dispFlash(txt));//(char*)0x8cc08a0); //Press STart Button string in crazy taxi binary
		//Draw Text
		rgb = 0xFD00;
		bfont_draw_large_str(vram_s+BUFFER_POS(10,10),640,"NEODC");
		bfont_draw_large_str(vram_s+BUFFER_POS(202,10),640,"RULES 2018");
		rgb = 0xFFFF;
		
		bfont_draw_large_str(vram_s+BUFFER_POS(64,400),640,"PRESS");
		bfont_draw_large_str(vram_s+BUFFER_POS(320,400),640,"TO PATCH");
		
		bfont_draw_large_str(vram_s+BUFFER_POS(32,440),640,"PRESS");
		bfont_draw_large_str(vram_s+BUFFER_POS(272,440),640,"TO BOOT RAW");
		
		//Draw Symbols
		//TM at top
		rgb = RGB(0,255,0);
		bfont_draw_mid_symbol(vram_s+BUFFER_POS(170,10),518400+(2*72));
		rgb = 0xFFFF;

		//A button
		rgb = RGB(255,30,30);
		if(flashing)
		bfont_draw_mid_symbol(vram_s+BUFFER_POS(256,404),518400+(11*72));
		rgb= 0xFFFF;
		
		//B Button
		rgb = RGB(0,155,211);
		if(flashing)
		bfont_draw_mid_symbol(vram_s+BUFFER_POS(224,444),518400+(12*72));
		rgb=0xffff;

		
		sleep_ms(333);

		//draw_string(10,440,"NeoDC Dev 2018",0xf0ff);
		flashing = !flashing;
		/*draw_string(375,300,"1",0xf0ff);
		draw_string(375,330,"2",0xf0ff);
		draw_string(375,380,"3",0xf0ff);
		draw_string(375,410,"4",0xf0ff);
		
		
		draw_string(400,300,(char*)(0xac010000+0x3FF85),0xf0ff);
		draw_string(400,330,(char*)(0xace00000+0x3FF85),0xffff);
		draw_string(400,380,(char*)(0x8c010000+0x3FF85),0xf0ff);
		draw_string(400,410,(char*)(0x8ce00000+0x3FF85),0xffff);
		*/
		if(!update_input(CONT_A)){
			draw_string(10,100,"scrambling!",0xf0bf);
			fixbin(2);
			break;
		}
		/*if(!update_input(CONT_Y)){
			draw_string(10,100,"unscrambling!",0xfbb0);
			fixbin(1);
			break;
		}*/
		if(!update_input(CONT_B)){
			//draw_string(10,100,"doing nothing!",0xfbb0);
			//fixbin(0);
			break;
		}
		//sleep_ms(100);
	} while(controls);
	//clrscr(0);
	/*draw_string(50,70,(char*)(0xac010000+0x3FF85),0xf0ff);
		draw_string(375,300,"1.",0x00ff);
		draw_string(375,330,"2.",0x00ff);
		draw_string(375,380,"3.",0x00ff);
		draw_string(375,410,"4.",0x00ff);
		
		
		draw_string(400,300,(char*)(0xac010000+0x3FF85),0xf0ff);
		draw_string(400,330,(char*)(0xace00000+0x3FF85),0xffff);
		draw_string(400,380,(char*)(0x8c010000+0x3FF85),0xf0ff);
		draw_string(400,410,(char*)(0x8ce00000+0x3FF85),0xffff);*/
		
	draw_string(20,130,"Done! START to boot",0xffff);
	draw_string(20,160,"Press X for crazy taxi patch",0xffff);
	//draw_string(400,76,"BP 0x8c00c620",0xffff);
	controls=1;
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
	} while(controls);
	start2();
}

