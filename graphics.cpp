#include "enums.h"
#include "globals.h"

#include "curses.h"

chtype comp(unsigned char sym, colorName color){

	chtype r = sym | color_value[color];
	return r;

};
