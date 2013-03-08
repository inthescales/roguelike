#include "actclass.h"
#include "globals.h"
#include "graphics.h"

#define ACT( name, ai, image, color ) \
	{ name, ai, image, color }

actclass aclass[] = {

	//#HUMAN
	ACT( "human", 0, '@', C_WHITE),

	//#ROBOT
	ACT( "robot", 0, 'R', C_WHITE)
	
};
