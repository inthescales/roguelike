#include "classdefs.h"
#include "effect.h"
#include "actor.h"
#include "object.h"
#include "tile.h"
#include "enums.h"
#include "globals.h"
#include "window.h"

#include <sstream>
using std::ostringstream;

effect::effect(effect_t e){

	target = TAR_NONE;
	radius = RAD_NONE;
	type = e;
}

effect::effect(target_t t, radius_t r, effect_t e){

	target = t;
	radius = r;
	type = e;
}

trigger_effect::trigger_effect(trigger_t tr, effect e) : trigger(tr), eff(e) {

}

/*
	Process an effect
	Takes in some relevant arguments
*/
bool do_effect(argmap * args, effect * eff){

	switch(eff->type){
    
        case EFF_TURN:
        {
            actor * agent = args->get_actor(ARG_AGENT);
            if(agent != NULL) {
                int post_time = agent->take_turn();
                agent->queue_turn(post_time);
            }
            
            break;
        }
            
		case EFF_BREAD:
			if(args->get_actor(ARG_AGENT) == act_player)
				win_output->print("This bread is delicious!");
			//act->taste(TST_BREAD, true);
			//act->nourish(obj->bonus);
			break;
            
        case EFF_PRINT_INT:
        {
            ostringstream convert;
			convert << args->get_int((args_t)0);
            win_output->print("TIMER DEBUG 1");
            getch();
        }
            break;
            
        case EFF_PRINT_INT_2:
        {
            ostringstream convert;
			convert << args->get_int((args_t)0);
            win_output->print("TIMER DEBUG 2");
            getch();
        }
            break;
            
        case EFF_PRINT_INT_3:
        {
            ostringstream convert;
			convert << args->get_int((args_t)0);
            win_output->print("TIMER DEBUG 3");
            getch();
        }
            break;
            
		default:
			break;		
	}
}