#include "tro.h"
#include "maple.h"

#define MAX_OPTIONS 4

void strcpy(char* dest, char* src){
	int i=0;
	while(src[i] != '\0'){
		dest[i]=src[i++];
	}
	dest[i]=0;
}

struct a_menu {
	char title[0x2e];
	int num_of_options;
	char options[MAX_OPTIONS][20];
};


struct a_menu *new_menu (char *title) {
  //struct a_menu *menu = (struct a_menu *) malloc (sizeof (struct a_menu));
  memset(0xACDAF000,sizeof (struct a_menu));
  struct a_menu *menu = (struct a_menu *)0xACDAF000;
  int i;

  strcpy (menu -> title, title);

  for (i = 0; i < MAX_OPTIONS; i++)
    strcpy (menu -> options[i], "");
  
  menu -> num_of_options = 0;

  return menu;
}


int add_option (struct a_menu *menu, char *caption) {

  if (menu -> num_of_options == MAX_OPTIONS)
    return -1;

  strcpy (menu -> options [menu -> num_of_options], caption);
  menu -> num_of_options++;

  return (menu -> num_of_options - 1);    //returns the array posn of the caption
}


int display_menu (struct a_menu *menu, int x, int y, int box_flag) {
  
  int i, y2 = y, button = 1, current = 0;

  draw_string (x, y, menu -> title,0x0000);
  y += 36;

  for (i = 0; i < menu -> num_of_options; i++) {
    draw_string (x, y, menu -> options[i],0x0000);
    y += 24;
  }

  y = y2 + 48;

  v_circle (x - 10, y, 8, 0, 255, 0);

  
  while ( button/*(button != btnX) && (button != btnSTART)*/ ) {
    button = update_input(CONT_A);
	
    //switch (button) {
    //case dpadUP: {
	if(!update_input(CONT_DPAD_UP)){
      if (current > 0) {
	v_circle (x - 10, y, 8, 0, 0, 0);
	y -= 24;
	v_circle (x - 10, y, 8, 0, 255, 0);
	current --;
      }
      else {
	v_circle (x - 10, y, 8, 0, 0, 255);
	sleep_ms(60);
	v_circle (x - 10, y, 8, 0, 255, 0);
      }
    //  break;
    }
    //case dpadDOWN: {
	if(!update_input(CONT_DPAD_DOWN)){
      if (current < (menu -> num_of_options - 1)) {
	v_circle (x - 10, y, 8, 0, 0, 0);
	y += 24;
	v_circle (x - 10, y, 8, 0, 255, 0);
	current ++;
      }
      else {
	v_circle (x - 10, y, 8, 0, 0, 255);
	sleep_ms(60);
	v_circle (x - 10, y, 8, 0, 255, 0);
      }
    //  break;
    }
    //default: {
      v_circle (x - 10, y, 8, 0, 0, 255);
      sleep_ms(60);
      v_circle (x - 10, y, 8, 0, 255, 0);
    //  break;
	
  }  //end of while (button != btnX)
  
  return current;
}

