#ifndef FEATCLASS_H

#define FEATCLASS_H

#include "enums.h"

#include <vector>
#include <curses.h>

class featclass {

	public:
		
	unsigned int can_walk : 1;
	unsigned int can_swim : 1;
	unsigned int can_fly : 1;
	unsigned int opaque : 1;

	unsigned char symbol;
	colorName color;
	
	//std::vector<effect> effects;
	
};

#endif
