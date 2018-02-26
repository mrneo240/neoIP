#include "tro.h"
#include "maple.h"

#define MAX_OPTIONS 4

void strcpy(char* dest, char* src)
{
	int i=0;
	while(src[i] != '\0' ) {
		dest[i]=src[i++];
	}
	dest[i]=0;
}

struct a_menu {
	char title[0x2e];
	int num_of_options;
	char options[MAX_OPTIONS][24];
	char enabled[MAX_OPTIONS];
};


struct a_menu *new_menu (char *title)
{
	//struct a_menu *menu = (struct a_menu *) malloc (sizeof (struct a_menu));
	memset(0xACDAF000,sizeof (struct a_menu));
	struct a_menu *menu = (struct a_menu *)0xACDAF000;
	int i;

	strcpy (menu -> title, title);

	for (i = 0; i < MAX_OPTIONS; i++) {
		//strcpy (menu -> options[i], "");
		menu -> enabled[i] = 0;
	}

	menu -> num_of_options = 0;

	return menu;
}


int add_option (struct a_menu *menu, char *caption)
{

	if (menu -> num_of_options == MAX_OPTIONS)
		return -1;

	strcpy (menu -> options [menu -> num_of_options++], caption);

	return (menu -> num_of_options - 1);    //returns the array posn of the caption
}

int current = 0;
int display_menu (struct a_menu *menu, int x, int y, int box_flag)
{

	int i, y2 = y, button = 1;
	#define LINE_HEIGHT 28
	int selected  = -1;

	draw_string (x, y, menu -> title,0x0000);
	y += LINE_HEIGHT*1.5;

	for (i = 0; i < menu -> num_of_options; i++) {
		draw_string (x, y, menu -> options[i],0x0000);
		draw_string (x+300, y, (menu->enabled[i] ? "ON" : "OFF"),(menu->enabled[i] ? 0x87F0 : 0xFC10));
		y += LINE_HEIGHT;
	}

	y = y2 + (LINE_HEIGHT*2);
	v_circle (x - 10, y+(current*LINE_HEIGHT), 8, 0, 255, 0);

	if(!update_input(CONT_DPAD_UP)) {
		if (current > 0) {
			v_circle (x - 10, y+(current*LINE_HEIGHT), 8, 0, 0, 0);
			v_circle (x - 10, y+(current*LINE_HEIGHT), 8, 0, 255, 0);
			current --;
		}
		else {
			v_circle (x - 10, y+(current*LINE_HEIGHT), 8, 0, 0, 255);
			v_circle (x - 10, y+(current*LINE_HEIGHT), 8, 0, 255, 0);
		}
	}

	if(!update_input(CONT_DPAD_DOWN)) {
		if (current < (menu -> num_of_options - 1)) {
			v_circle (x - 10, y+(current*LINE_HEIGHT), 8, 0, 0, 0);
			v_circle (x - 10, y+(current*LINE_HEIGHT), 8, 0, 255, 0);
			current ++;
		}
		else {
			v_circle (x - 10, y+(current*LINE_HEIGHT), 8, 0, 0, 255);
			v_circle (x - 10, y+(current*LINE_HEIGHT), 8, 0, 255, 0);
		}
	}
	v_circle (x - 10, y+(current*LINE_HEIGHT), 8, 0, 0, 255);
	v_circle (x - 10, y+(current*LINE_HEIGHT), 8, 0, 255, 0);

	if(!update_input(CONT_A)) {
		menu->enabled[current] = !menu->enabled[current];
		selected = current+1;
	}

	//}  //end of while (button != btnX)


	return selected;
}

