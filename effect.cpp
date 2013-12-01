#include "classdefs.h"
#include "effect.h"
#include "actor.h"
#include "object.h"
#include "tile.h"
#include "enums.h"
#include "globals.h"
#include "window.h"

effect::effect(target_t t, radius_t r, effect_t e){

	target = t;
	radius = r;
	type = e;
}

trigger_effect::trigger_effect(trigger_t tr, effect e) : trigger(tr), eff(e) {

}

/*
	Process an effect
	Takes in some relevant agents
*/
bool do_effect(argmap * args, effect * eff){

	switch(eff->type){
		case EFF_BREAD:
			if(args->get_actor(ARG_AGENT) == act_player)
				win_output->print("This bread is delicious!");
			//act->taste(TST_BREAD, true);
			//act->nourish(obj->bonus);
			break;
		default:
			break;		
	}
}