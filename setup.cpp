#include "actdefs.h"
#include "conddefs.h"
#include "config.h"
#include "enums.h"
#include "globals.h"
#include "interface.h"
#include "objdefs.h"
#include "settings.h"
#include "setup.h"
#include "tiledefs.h"
#include "window.h"

#include LIB_CURSES

#include <stdlib.h>
#include <time.h>
#include <string>
#include <vector>
#include <clocale>

using std::string;
using std::vector;

void setup(){

    setlocale(LC_ALL, "en_US.UTF-8");
	initscr();
	resize_term(win_output->height + win_world->height + win_status->height, win_output->width);
	keypad(stdscr, TRUE);
	setup_color();
    setup_ui();
	cbreak();
	noecho();
	curs_set(1);
	srand( time( NULL ) );

	define_symbols();
	define_actors();
	define_objects();
	define_tiles();
	define_conditions();
	
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

		init_pair(CP_BLACK_BLACK, COLOR_BLACK, COLOR_BLACK);
		init_pair(CP_GRAY_BLACK, COLOR_WHITE, COLOR_BLACK);
		init_pair(CP_RED_BLACK, COLOR_RED, COLOR_BLACK);
		init_pair(CP_GREEN_BLACK, COLOR_GREEN, COLOR_BLACK);
		init_pair(CP_BLUE_BLACK, COLOR_BLUE, COLOR_BLACK);
		init_pair(CP_PURPLE_BLACK, COLOR_MAGENTA, COLOR_BLACK);
		init_pair(CP_BROWN_BLACK, COLOR_YELLOW, COLOR_BLACK);
		init_pair(CP_TEAL_BLACK, COLOR_CYAN, COLOR_BLACK);
		init_pair(CP_BLACK_GRAY, COLOR_BLACK, COLOR_WHITE);
	}
}

void setup_ui() {
    UI::setup_ui();
}
