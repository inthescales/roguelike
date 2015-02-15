#include "classdefs.h"
#include "effect.h"
#include "actor.h"
#include "object.h"
#include "feature.h"
#include "enums.h"
#include "globals.h"
#include "mapentity.h"
#include "stringutils.h"
#include "tile.h"
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
    
    bool ok = true;
    bool success = false;
    vector<entity*> * successes = new vector<entity*>();
    string output;
    
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
        break;
        
        case EFF_WALK:
        {
            actor * agent = args->get_actor(ARG_ACTION_AGENT);
            tile * patient = (tile *)args->get_vector(ARG_ACTION_PATIENT)->front();
            
            agent->walk(patient);
            
            return success;
        }
        break;
        
        case EFF_SWIM:
        {
            actor * agent = args->get_actor(ARG_ACTION_AGENT);
            tile * patient = (tile *)args->get_vector(ARG_ACTION_PATIENT)->front();
            
            agent->swim(patient);
            
            return success;
        }
        break;
        
        case EFF_FLY:
        {
            actor * agent = args->get_actor(ARG_ACTION_AGENT);
            tile * patient = (tile *)args->get_vector(ARG_ACTION_PATIENT)->front();
            
            agent->fly(patient);
            
            return success;
        }
        break;
        
        case EFF_TAKE:
        {
            actor * agent = args->get_actor(ARG_ACTION_AGENT);
            vector<object*> * patients = (vector<object*>*)(args->get_vector(ARG_ACTION_PATIENT));

            vector<object*>::iterator it = patients->begin();
            for (; ok && it != patients->end(); ++it) {
            
                ok = agent->pick_up(*it);
                if (ok)successes->push_back((entity*)(*it));
            }
            
            if(successes->size() == 1){
                output = "You take the " + ((mapentity *)successes->back())->get_name_color() + ".";
            } else if (successes->size() > 1 && successes->size() <= 5) {
                output = "You take: ";
                for(int i = 0; i < successes->size(); ++i) {
                    output += ((mapentity *)successes->at(i))->get_name_color();
                    if (i != successes->size() - 1) {
                        output += ", ";
                    }
                }
            } else if (successes->size() > 0) {
                output = "You take " + int_string(successes->size()) + " objects";
            }
            
            win_output->print(output);
            
            return success;
        }
        break;
        
        case EFF_DROP:
        {
            actor * agent = args->get_actor(ARG_ACTION_AGENT);
            vector<object*> * patients = (vector<object*>*)(args->get_vector(ARG_ACTION_PATIENT));

            vector<object*>::iterator it = patients->begin();
            for (; ok && it != patients->end(); ++it) {            
                ok = agent->drop(*it);
                if (ok) {
                    successes->push_back((entity*)(*it));
                }
            }
            
            if(successes->size() == 1){
                output = "You drop the " + ((mapentity *)successes->back())->get_name_color() + ".";
            } else if (successes->size() > 1 && successes->size() <= 5) {
                output = "You drop: ";
                for(int i = 0; i < successes->size(); ++i) {
                    output += ((mapentity *)successes->at(i))->get_name_color();
                    if (i != successes->size() - 1) {
                        output += ", ";
                    }
                }
            } else if (successes->size() > 0) {
                output = "You drop " + int_string(successes->size()) + " objects";
            }
            
            win_output->print(output);
        }
        break;
        
        case EFF_EQUIP:
        {
            actor * agent = args->get_actor(ARG_ACTION_AGENT);
            vector<object*> * patients = (vector<object*>*)(args->get_vector(ARG_ACTION_PATIENT));
            
            vector<object*>::iterator it = patients->begin();
            for (; ok && it != patients->end(); ++it) {
                ok = agent->equip(*it);
            }
        }
        break;
        
        case EFF_UNEQUIP:
        {
            actor * agent = args->get_actor(ARG_ACTION_AGENT);
            vector<object*> * patients = (vector<object*>*)(args->get_vector(ARG_ACTION_PATIENT));
            
            vector<object*>::iterator it = patients->begin();
            for (; ok && it != patients->end(); ++it) {
                ok = agent->unequip(*it);
            }
        }
        break;
        
        case EFF_EAT:
        {
            actor * agent = args->get_actor(ARG_ACTION_AGENT);
            object * patient = (object *)(args->get_vector(ARG_ACTION_PATIENT)->front());
            
            agent->eat(patient);
            
            return success;
        }
        break;
        
        case EFF_DRINK:
        {
            actor * agent = args->get_actor(ARG_ACTION_AGENT);
            object * patient = (object *)(args->get_vector(ARG_ACTION_PATIENT)->front());
            
            agent->drink(patient);
            
            return success;
        }
        break;
        
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
        
        case EFF_STRIKE:
        {
            actor * agent = args->get_actor(ARG_ACTION_AGENT);
            vector<actor*> * patients = (vector<actor*>*)(args->get_vector(ARG_ACTION_PATIENT));
            
            vector<actor*>::iterator it = patients->begin();
            for (; ok && it != patients->end(); ++it) {
                ok = agent->strike(*it);
            }
        }
        break;
        
        case EFF_PUNCH:
        {
            actor * agent = args->get_actor(ARG_ACTION_AGENT);
            vector<actor*> * patients = (vector<actor*>*)(args->get_vector(ARG_ACTION_PATIENT));
            
            vector<actor*>::iterator it = patients->begin();
            for (; ok && it != patients->end(); ++it) {
                ok = agent->punch(*it);
            }
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