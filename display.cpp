#include "color.h"
#include "enums.h"
#include "display.h"
#include "globals.h"
#include "glyph.h"
#include "stringutils.h"

#include <stdlib.h>
#include <stack>

using std::stack;

// Print a string, using color codes included
void printcolor(int x, int y, string in){

	int len = in.size();
    stack<colorName> colorStack;
    colorStack.push(C_WHITE);

	move(y, x);
	for(int i = 0; i < len; ++i){
	
		if(in.at(i) == color_escape_start){ 
            colorStack.push((colorName)(in.at(++i)));
		} else if (in.at(i) == color_escape_end) {
            colorStack.pop();
        } else {
            printchar_cw(in.at(i), colorStack.top());
		}
	}
}

// Print a glyph using its symbol and color
void printglyph(glyph gly) {
#if DISPLAY_TYPE == DISPLAY_CURSES \
 || DISPLAY_TYPE == DISPLAY_WCURSES
    printchar_cw(gly.get_symbol(), gly.get_color());
#endif
}

// Print a symbol in white
void printchar_cw(symbol_code s) {
  printchar_cw(s, C_WHITE);
}

// Print symbol in color
void printchar_cw(symbol_code sym, colorName color) {

#if DISPLAY_TYPE == DISPLAY_CURSES
  attron(COLOR_PAIR(color::get_pair(color)));
  if (color::get_bold(color)) attron(A_BOLD);
  if (color::get_blink(color)) attron(A_BLINK);
  addch(sym);
  attroff(COLOR_PAIR(color::get_pair(color)));
  if (color::get_bold(color)) attroff(A_BOLD);
  if (color::get_blink(color)) attroff(A_BLINK);
#elif DISPLAY_TYPE == DISPLAY_WCURSES
  cchar_t cc;
  wchar_t wc = sym;
  long col = color::get_pair(color);
  long attr = color::get_attr(color);

  setcchar(&cc, &wc, attr, col, NULL);
  add_wch(&cc);
#endif
  
}
