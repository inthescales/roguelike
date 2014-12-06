#ifndef TILECLASS_H

#define TILECLASS_H

#include "config.h"
#include "display.h"
#include "enums.h"
#include "mapentityclass.h"

struct tileclass : public mapentityclass {

	unsigned int can_walk : 1;
	unsigned int can_swim : 1;
	unsigned int can_fly : 1;
	unsigned int opaque : 1;

    tileclass(bool, bool, bool, bool, symbol_code, colorName);
};

#endif
