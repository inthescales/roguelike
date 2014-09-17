#ifndef GLYPH_H
#define GLYPH_H

#include "config.h"
#include "display.h"
#include "enums.h"

class glyph {

 public:
#if DISPLAY_TYPE == DISPLAY_CURSES || DISPLAY_TYPE == DISPLAY_WCURSES
  symbol_code symbol;
  colorName color;

  glyph();
  glyph(symbol_code, colorName);
  symbol_code get_symbol();
  colorName get_color();
#endif

};

#endif
