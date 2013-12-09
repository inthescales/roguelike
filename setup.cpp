#include <curses.h>
#include <stdlib.h>
#include <time.h>

#include "globals.h"
#include "settings.h"
#include "setup.h"
#include "window.h"

#include "actdefs.h"
#include "objdefs.h"

#include <string>
#include <vector>

using std::string;
using std::vector;

void setup(){

	initscr();
	resize_term(win_output->height + win_world->height + win_status->height, win_output->width);
	keypad(stdscr, TRUE);
	start_color();
	setup_color();
	cbreak();
	noecho();
	curs_set(1);
	srand( time( NULL ) );
	
	define_actors();
	define_objects();
	
	for(int i = 0; i < 52; ++i) obj_letter[i] = NULL;
}

void setup_window(){

	int width = 60;

	win_output = new window(0, 0, width, 4);
	win_world  = new window(0, win_output->y + win_output->height, width, 32);
	win_status = new window(0, win_world->y + win_world->height, width, 6);
	win_screen = new window(0, 0, width, win_status->y + win_status->height);
	buf_main = new vector<string>;
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
