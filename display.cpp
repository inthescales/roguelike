#include "color.h"
#include "enums.h"
#include "display.h"
#include "globals.h"

void printcolor(int x, int y, string in){

	int len = in.size();
	int color = C_WHITE;

	move(y, x);
	for(int i = 0; i < len; ++i){
	
		if(in.at(i) == '|'){ 
			color = in.at(i+1);
			++i;
		} else {
		  printchar_cw(in.at(i), color);
		}
	}
	
}

void printchar_cw(d_glyph gly) {
  printchar_cw(gly, C_WHITE);
}

void printchar_cw(d_glyph gly, int color) {
#if DISPLAY_TYPE == DISPLAY_CURSES
  addch(gly | color);
#elif DISPLAY_TYPE == DISPLAY_WCURSES
  cchar_t cc;
  wchar_t wc = gly;
  long col = get_color(color);
  long attr = get_attr(color);

  setcchar(&cc, &wc, attr, col, NULL);
  add_wch(&cc);
#endif
  
}

/*
Future ideas - have an image descriptor class that can be passed as an input to a print function.
This class would vary depending on graphics config - for curses would be char (ascii or unicode) plus
color, but might include other info for tiled versions (e.g. image file plus variant flags)
 */
