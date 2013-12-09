#include "actclass.h"
#include "actdefs.h"
#include "globals.h"
#include "graphics.h"

void define_actors(){

	int c = -1;

	//#HUMAN
	aclass[++c] = new actclass("human", '@', C_WHITE);
	aclass[c]->add_basic_stats(5, 5);
	
	//#ROBOT
	aclass[++c] = new actclass("robot", 'R', C_ORANGE);
	aclass[c]->add_basic_stats(8, 2);
};
