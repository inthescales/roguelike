#ifndef TILECLASS_H

#define TILECLASS_H

#include "enums.h"

#include <curses.h>

struct tileclass {

	unsigned int can_walk : 1;
	unsigned int can_swim : 1;
	unsigned int can_fly : 1;
	unsigned int opaque : 1;

	unsigned char symbol;
	colorName color;

};

#define TILE(walk, swim, fly , opaque, image, color) \
		{ walk, swim, fly, opaque, image, color }

#endif
