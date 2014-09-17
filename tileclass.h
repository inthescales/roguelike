#ifndef TILECLASS_H

#define TILECLASS_H

#include "config.h"
#include "display.h"
#include "glyph.h"
#include "enums.h"

struct tileclass {

	unsigned int can_walk : 1;
	unsigned int can_swim : 1;
	unsigned int can_fly : 1;
	unsigned int opaque : 1;

        glyph image;
  //symbol_code symbol;
  //colorName color;

  tileclass(bool, bool, bool, bool, symbol_code, colorName);
  glyph get_glyph();
  symbol_code get_symbol();
  colorName get_color();
};

#endif
