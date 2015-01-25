#include "classdefs.h"
#include "effect.h"
#include "actor.h"
#include "object.h"
#include "feature.h"
#include "tile.h"
#include "enums.h"
#include "globals.h"
#include "window.h"

effect::effect(effect_t e) {

	type = e;
}

trigger_effect::trigger_effect(effect_t e, trigger_t tr) : effect(e) {

    type = e;
    trigger = tr;
}

timer_effect::timer_effect(effect_t e, int n_time, int n_iters, int n_delta) : effect(e) {
    
    type = e;
    time = n_time;
    iterations = n_iters;
    delta = n_delta;
}

/*
	Process an effect
	Takes in some relevant arguments
*/
bool do_effect(argmap * args, effect * eff){

    if (eff == NULL)
        return false;
    
    bool success = false;
    
	switch(eff->type){
    
        case EFF_TURN:
        {
            actor * agent = args->get_actor(ARG_ACTION_AGENT);
            if(agent != NULL) {
                int post_time = agent->take_turn();
                agent->queue_turn(post_time);
            }
            
            return true;
        }
        
        case EFF_PICK_UP:
        {
            actor * agent = args->get_actor(ARG_ACTION_AGENT);
            object * patient = (object *)args->get_vector(ARG_ACTION_PATIENT)->front();
            
            agent->pick_up(patient);
            
            return success;
        }
        
        case EFF_EAT:
        {
            actor * agent = args->get_actor(ARG_ACTION_AGENT);
            object * patient = (object *)(args->get_vector(ARG_ACTION_PATIENT)->front());
            
            agent->eat(patient);
            
            return success;
        }
        
        case EFF_DRINK:
        {
            actor * agent = args->get_actor(ARG_ACTION_AGENT);
            object * patient = (object *)(args->get_vector(ARG_ACTION_PATIENT)->front());
            
            agent->drink(patient);
            
            return success;
        }
        
        case EFF_FEAT_OPEN:
        {
            actor * agent = args->get_actor(ARG_ACTION_AGENT);
            feature * patient = (feature *)(args->get_vector(ARG_ACTION_PATIENT)->front());
            
            agent->open_feature(patient);
        }
        break;
        
        case EFF_FEAT_CLOSE:
        {
            actor * agent = args->get_actor(ARG_ACTION_AGENT);
            feature * patient = (feature *)(args->get_vector(ARG_ACTION_PATIENT)->front());
            
            agent->close_feature(patient);
        }
        break;
        
        case EFF_COND_TIMEOUT:
        {
            //TODO
            break;
        }
        
        // object behavior
        
		case EFF_EAT_BREAD:
			if(args->get_actor(ARG_ACTION_AGENT) == act_player)
				win_output->print("The bread is delicious!");
			//act->taste(TST_BREAD, true);
			//act->nourish(obj->bonus);
			break;
            
        case EFF_DRINK_WATER:
            if(args->get_actor(ARG_ACTION_AGENT) == act_player)
				win_output->print("The water is refreshing.");
			break;
            
		default:
			break;		
	}
}