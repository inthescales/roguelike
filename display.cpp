#include "color.h"
#include "enums.h"
#include "display.h"
#include "globals.h"
#include "glyph.h"

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
	
		if(in.at(i) == '|'){ 
            colorStack.push((colorName)(in.at(++i)));
		} else if (in.at(i) == '~') {
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
  attron(COLOR_PAIR(get_color(color)));
  if (get_bold(color)) attron(A_BOLD);
  addch(sym);
  attroff(COLOR_PAIR(get_color(color)));
  if (get_bold(color)) attroff(A_BOLD);
#elif DISPLAY_TYPE == DISPLAY_WCURSES
  cchar_t cc;
  wchar_t wc = sym;
  long col = get_color(color);
  long attr = get_attr(color);

  setcchar(&cc, &wc, attr, col, NULL);
  add_wch(&cc);
#endif
  
}
