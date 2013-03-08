#include <curses.h>
#include <stdlib.h>
#include <time.h>

#include "globals.h"
#include "setup.h"
#include "window.h"

void setup(){

	initscr();
	resize_term(win_output->height + win_world->height + win_status->height, win_output->width);
	keypad(stdscr, TRUE);
	start_color();
	cbreak();
	noecho();
	curs_set(1);
	srand( time( NULL ) );
}

void setup_window(){

	win_output = new window(0, 0, 60, 4);
	win_world  = new window(0, win_output->y + win_output->height, 60, 32);
	win_status = new window(0, win_world->y + win_world->height, 60, 6);
	
	//buf_main = new buffer();
}

void setup_color(){
	if(has_colors())
	{
		start_color();

		init_pair(0, COLOR_BLACK, COLOR_BLACK);
		init_pair(1, COLOR_WHITE, COLOR_BLACK);
		init_pair(2, COLOR_RED, COLOR_BLACK);
		init_pair(3, COLOR_GREEN, COLOR_BLACK);
		init_pair(4, COLOR_BLUE, COLOR_BLACK);
		init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
		init_pair(6, COLOR_YELLOW, COLOR_BLACK);
		init_pair(7, COLOR_CYAN, COLOR_BLACK);
	}
}
