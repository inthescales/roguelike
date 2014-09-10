#ifndef TILECLASS_H

#define TILECLASS_H

#include "config.h"
#include "display.h"
#include "enums.h"

#include LIB_CURSES

struct tileclass {

	unsigned int can_walk : 1;
	unsigned int can_swim : 1;
	unsigned int can_fly : 1;
	unsigned int opaque : 1;

	d_glyph symbol;
	colorName color;

};

#define TILE(walk, swim, fly , opaque, image, color) \
		{ walk, swim, fly, opaque, image, color }

#endif
