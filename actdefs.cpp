#include "actclass.h"
#include "actdefs.h"
#include "display.h"
#include "globals.h"

void define_actors(){

	int c = -1;

	//#HUMAN
	aclass[++c] = new actclass("human", '@', C_WHITE);
	aclass[c]->stats->add_stat(ASTAT_BODY, 5);
	aclass[c]->stats->add_stat(ASTAT_MIND, 5);	
	
	//#ROBOT
	aclass[++c] = new actclass("robot", 'R', C_ORANGE);
	aclass[c]->stats->add_stat(ASTAT_BODY, 8);
	aclass[c]->stats->add_stat(ASTAT_MIND, 2);
};
