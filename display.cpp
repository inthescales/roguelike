#include "color.h"
#include "enums.h"
#include "display.h"
#include "globals.h"
#include "glyph.h"

void printcolor(int x, int y, string in){

	int len = in.size();
	colorName color = C_WHITE;

	move(y, x);
	for(int i = 0; i < len; ++i){
	
		if(in.at(i) == '|'){ 
		  color = (colorName)(in.at(i+1));
			++i;
		} else {
		  printchar_cw(in.at(i), color);
		}
	}
	
}

void printglyph(glyph gly) {
#if DISPLAY_TYPE == DISPLAY_CURSES || DISPLAY_TYPE == DISPLAY_WCURSES
    printchar_cw(gly.get_symbol(), gly.get_color());
#endif
}

void printchar_cw(symbol_code s) {
  printchar_cw(s, C_WHITE);
}

void printchar_cw(symbol_code sym, colorName color) {
#if DISPLAY_TYPE == DISPLAY_CURSES
  addch(sym | color);
#elif DISPLAY_TYPE == DISPLAY_WCURSES
  cchar_t cc;
  wchar_t wc = sym;
  long col = get_color(color);
  long attr = get_attr(color);

  setcchar(&cc, &wc, attr, col, NULL);
  add_wch(&cc);
#endif
  
}
