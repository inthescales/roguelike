#include "featclass.h"
#include "featdefs.h"
#include "display.h"
#include "symboldefs.h"

void define_features(){

	int c = -1;
    effect * new_effect;
    
	//#CLOSED_WOODEN_DOOR
	fclass[++c] = new featclass("wooden door", symboldef[SYM_DOORCLOSED], C_BROWN);
    fclass[c]->add_flag(FLAG_FEAT_OPAQUE);
    fclass[c]->add_flag(FLAG_FEAT_NO_WALK);
    fclass[c]->add_flag(FLAG_FEAT_NO_SWIM);
    fclass[c]->add_flag(FLAG_FEAT_NO_FLY);
    fclass[c]->add_flag(FLAG_FEAT_CAN_OPEN);
    fclass[c]->add_trigger_effect(new trigger_effect(EFF_FEAT_OPEN, TRG_OPEN));
    
    //#OPEN_WOODEN_DOOR
    fclass[++c] = new featclass("wooden door", symboldef[SYM_DOOROPEN], C_BROWN);
    fclass[c]->add_flag(FLAG_FEAT_CAN_CLOSE);
    fclass[c]->add_trigger_effect(new trigger_effect(EFF_FEAT_CLOSE, TRG_CLOSE));
    
    // link doors
    fclass[c]->add_state(STATE_FEAT_CLOSED, fclass[c-1]);
    fclass[c-1]->add_state(STATE_FEAT_OPEN, fclass[c]);
    
    //#TREE
    fclass[++c] = new featclass("tree", symboldef[SYM_TREE], C_GREEN);
    fclass[c]->add_flag(FLAG_FEAT_OPAQUE);
    fclass[c]->add_flag(FLAG_FEAT_NO_WALK);
    fclass[c]->add_flag(FLAG_FEAT_NO_SWIM);
    fclass[c]->add_flag(FLAG_FEAT_NO_FLY);
};
