#include "enums.h"
#include "globals.h"
#include "graphics.h"

#include "curses.h"

chtype comp(unsigned char sym, colorName color){

	chtype r = sym | color_value[color];
	return r;

};

void printcolor(int x, int y, string in){

	int len = in.size();
	int color = C_WHITE;

	move(y, x);
	for(int i = 0; i < len; ++i){
	
		if(in.at(i) == '|'){ 
			color = in.at(i+1);
			++i;
		} else {
			addch(in.at(i) | color_value[color]);
		}
	}
	
}