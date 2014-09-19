#include "actclass.h"

actclass::actclass(string n, symbol_code s, colorName c) {

	name = n;
#if DISPLAY_TYPE == DISPLAY_CURSES || DISPLAY_TYPE == DISPLAY_WCURSES
       	image = glyph(s, c);
#endif
	stats = new statmap();
}

void actclass::add_effect(trigger_effect n){

	effects.push_back(n);
}

glyph actclass::get_glyph() {
  return image;
}

symbol_code actclass::get_symbol() {
  return image.get_symbol();
}


colorName actclass::get_color() {
  return image.get_color();
}
