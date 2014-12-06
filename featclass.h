#ifndef FEATCLASS_H

#define FEATCLASS_H

#include "config.h"
#include "enums.h"
#include "mapentityclass.h"

#include <vector>

class featclass : public mapentityclass {

	public:
	
    // TODO - change these to flags
	unsigned int can_walk : 1;
	unsigned int can_swim : 1;
	unsigned int can_fly : 1;
	unsigned int opaque : 1;
	
};

#endif
