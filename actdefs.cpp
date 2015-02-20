#include "actclass.h"
#include "actdefs.h"
#include "classdefs.h"
#include "display.h"
#include "symboldefs.h"

void define_actors(){

	int c = -1;

	//#HUMAN
	aclass[++c] = new actclass("human", symboldef[SYM_PLAYER], C_WHITE);
	aclass[c]->add_stat(ASTAT_BODY, 5);
	aclass[c]->add_stat(ASTAT_MIND, 5);	
    aclass[c]->add_stat(ASTAT_VISION_RANGE, 8);
	aclass[c]->add_flag(FLAG_ACT_CAN_WALK);
    aclass[c]->add_action(ACTION_WALK_BASIC);
    aclass[c]->add_action(ACTION_TAKE_BASIC);
    aclass[c]->add_action(ACTION_DROP_BASIC);
    aclass[c]->add_action(ACTION_EAT_BASIC);
    aclass[c]->add_action(ACTION_DRINK_BASIC);
    aclass[c]->add_action(ACTION_OPEN_BASIC);
    aclass[c]->add_action(ACTION_CLOSE_BASIC);
    aclass[c]->add_action(ACTION_STRIKE);
    aclass[c]->add_action(ACTION_PUNCH);
    
	//#ROBOT
	aclass[++c] = new actclass("robot", symboldef[SYM_ROBOT], C_ORANGE);
	aclass[c]->add_stat(ASTAT_BODY, 8);
	aclass[c]->add_stat(ASTAT_MIND, 2);
    aclass[c]->add_stat(ASTAT_VISION_RANGE, 8);
    aclass[c]->add_flag(FLAG_ACT_CAN_WALK);
    aclass[c]->add_action(ACTION_WALK_BASIC);
    aclass[c]->add_action(ACTION_TAKE_BASIC);
    aclass[c]->add_action(ACTION_DROP_BASIC);
    aclass[c]->add_action(ACTION_OPEN_BASIC);
    aclass[c]->add_action(ACTION_CLOSE_BASIC);
    aclass[c]->add_action(ACTION_STRIKE);
    aclass[c]->add_action(ACTION_PUNCH);
};
