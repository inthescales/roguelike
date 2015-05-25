#include "classdefs.h"
#include "effect.h"
#include "actor.h"
#include "object.h"
#include "feature.h"
#include "enums.h"
#include "mapentity.h"
#include "rogue.h"
#include "stringutils.h"
#include "tile.h"
#include "window.h"

effect::effect(effect_t e) {

	type = e;
    args = new argmap();
}

trigger_effect::trigger_effect(effect_t e, trigger_t tr) : effect(e) {

    type = e;
    trigger = tr;
    args = new argmap();
}

timer_effect::timer_effect(effect_t e, int n_time, int n_iters, int n_delta) : effect(e) {
    
    type = e;
    time = n_time;
    iterations = n_iters;
    delta = n_delta;
    args = new argmap();
}

// --------------

effect_resp::effect_resp() {
    
    result = EFFRES_UNKNOWN;
    successes = new vector<void*>();
    errors = new vector<error*>();
}

/*
    Merge two effect responses.
    Result has intersection of successes, union of errors.
    State is set accordingly.
*/
void effect_resp::merge(effect_resp * other) {

    // Keep my successes only if the other agrees
    vector<void*> * new_succs = new vector<void*>();
    vector<void*>::iterator it = successes->begin();
    vector<void*>::iterator it2 = other->successes->begin();
    for(; it != successes->end(); ++it) {
        for(; it2 != other->successes->end(); ++it2) {
        
            if((*it) == (*it2)) {
                new_succs->push_back(*it);
                break;
            }
        }
    }

    // Add other's errors to my own - they won't overlap
    vector<error*>::iterator err_it = other->errors->begin();
    for(; err_it != other->errors->end(); ++err_it) {
        errors->push_back(*err_it);
    }
    
    successes = new_succs;
        
    if (errors->size() > 0) {
        if (successes->size() > 0) {
            result = EFFRES_PARTIAL;
        } else {
            result = EFFRES_FAILURE;
        }        
    } else {
        result = EFFRES_SUCCESS;
    }
}

// ===============================================

// Resolve an effect object
effect_resp * effect::resolve(argmap * instanceArgs) {

    return effect::resolve(type, args, instanceArgs);
}

effect_resp * effect::resolve() {

    return resolve(NULL);
}

/*
    Resolve an effect with a given type and args map
*/
effect_resp * effect::resolve(effect_t type, argmap * args, argmap * instanceArgs){
    
    effect_resp * response = new effect_resp;
    bool ok = true;
    string output;
    
	switch(type){
    
        /*
            Take a turn as an actor
            Agent: Actor to act
        */
        case EFF_TURN:
        {
            actor * agent = (actor *)instanceArgs->get_from_vector(ARG_ACTION_AGENT);
            if(agent != NULL) {
                int post_time = agent->take_turn();
                agent->queue_turn(post_time);
                response->successes->push_back((void*)agent);
            }     
        }
        break;
        
        case EFF_WAIT:
        {
            actor * agent = (actor *)instanceArgs->get_from_vector(ARG_ACTION_AGENT);
            response->successes->push_back((void*)agent);
        }
        break;
        
        /*
            Move an actor to a new tile.
            Agent: Actor moving
            Location: Destination tile
        */
        case EFF_MOVE_ACT:
        {
            actor * agent = (actor *)instanceArgs->get_from_vector(ARG_ACTION_AGENT);
            tile * location = (tile *)instanceArgs->get_vector(ARG_ACTION_LOCATION)->front();
            
            if (agent != NULL && location != NULL) {
                agent->move(location);
                response->successes->push_back((void*)agent);
            }
        }
        break;
        
        case EFF_TAKE:
        {
            actor * agent = (actor *)instanceArgs->get_from_vector(ARG_ACTION_AGENT);
            vector<object*> * patients = (vector<object*>*)(instanceArgs->get_vector(ARG_ACTION_PATIENT));

            vector<object*>::iterator it = patients->begin();
            for (; ok && it != patients->end(); ++it) {
            
                ok = agent->pick_up(*it);
                if (ok) {
                    response->successes->push_back((void*)(*it));
                } else {
                    response->errors->push_back(new error(ERR_CANT_CARRY));
                    // Should return a specific error code
                }
            }
            
            if(response->successes->size() == 1){
                output = "You take the " + ((mapentity *)response->successes->back())->get_name_color() + ".";
            } else if (response->successes->size() > 1 && response->successes->size() <= 3) {
                output = "You take: ";
                for(int i = 0; i < response->successes->size(); ++i) {
                    output += ((mapentity *)response->successes->at(i))->get_name_color();
                    if (i != response->successes->size() - 1) {
                        output += ", ";
                    }
                }
            } else if (response->successes->size() > 0) {
                output = "You take " + int_string(response->successes->size()) + " objects";
            }
            
            win_output->print(output);
        }
        break;
        
        case EFF_DROP:
        {
            actor * agent = (actor *)instanceArgs->get_from_vector(ARG_ACTION_AGENT);
            vector<object*> * patients = (vector<object*>*)(instanceArgs->get_vector(ARG_ACTION_PATIENT));

            vector<object*>::iterator it = patients->begin();
            for (; ok && it != patients->end(); ++it) {            
                ok = agent->drop(*it);
                if (ok) {
                    response->successes->push_back((void*)(*it));
                } else {
                    response->errors->push_back(new error(ERR_GENERIC));
                    // Should return a specific error code
                }
            }
            
            if(response->successes->size() == 1){
                output = "You drop the " + ((mapentity *)response->successes->back())->get_name_color() + ".";
            } else if (response->successes->size() > 1 && response->successes->size() <= 3) {
                output = "You drop: ";
                for(int i = 0; i < response->successes->size(); ++i) {
                    output += ((mapentity *)response->successes->at(i))->get_name_color();
                    if (i != response->successes->size() - 1) {
                        output += ", ";
                    }
                }
            } else if (response->successes->size() > 0) {
                output = "You drop " + int_string(response->successes->size()) + " objects";
            }
            
            win_output->print(output);
        }
        break;
        
        case EFF_EQUIP:
        {
            actor * agent = (actor *)instanceArgs->get_from_vector(ARG_ACTION_AGENT);
            vector<object*> * patients = (vector<object*>*)(instanceArgs->get_vector(ARG_ACTION_PATIENT));
            
            vector<object*>::iterator it = patients->begin();
            for (; ok && it != patients->end(); ++it) {
                ok = agent->equip(*it);
                if (ok) {
                    response->successes->push_back((void*)(*it));
                } else {
                    response->errors->push_back(new error(ERR_GENERIC));
                    // Should return a specific error code
                }
            }
        }
        break;
        
        case EFF_UNEQUIP:
        {
            actor * agent = (actor *)instanceArgs->get_from_vector(ARG_ACTION_AGENT);
            vector<object*> * patients = (vector<object*>*)(instanceArgs->get_vector(ARG_ACTION_PATIENT));
            
            vector<object*>::iterator it = patients->begin();
            for (; ok && it != patients->end(); ++it) {
            
                ok = agent->unequip(*it);
                if (ok) {
                    response->successes->push_back((void*)(*it));
                } else {
                    response->errors->push_back(new error(ERR_GENERIC));
                    // Should return a specific error code
                }
            }
        }
        break;
        
        case EFF_EAT:
        {
            actor * agent = (actor *)instanceArgs->get_from_vector(ARG_ACTION_AGENT);
            vector<object*> * patients = (vector<object*> *)(instanceArgs->get_vector(ARG_ACTION_PATIENT));
            
            vector<object*>::iterator it = patients->begin();
            for(; it != patients->end(); ++it) {
            
                ok = agent->eat(*it);
                if (ok) {
                    response->successes->push_back((void*)(*it));
                } else {
                    response->errors->push_back(new error(ERR_GENERIC));
                    // Should return a specific error code
                }
            }
        }
        break;
        
        case EFF_DRINK:
        {
            actor * agent = (actor *)instanceArgs->get_from_vector(ARG_ACTION_AGENT);
            vector<object*> * patients = (vector<object*> *)(instanceArgs->get_vector(ARG_ACTION_PATIENT));
            
            vector<object*>::iterator it = patients->begin();
            for(; it != patients->end(); ++it) {
            
                ok = agent->drink(*it);
                if (ok) {
                    response->successes->push_back((void*)(*it));
                } else {
                    response->errors->push_back(new error(ERR_GENERIC));
                    // Should return a specific error code
                }
            }
        }
        break;
        
        case EFF_FEAT_OPEN:
        {
            actor * agent = (actor *)instanceArgs->get_from_vector(ARG_ACTION_AGENT);
            vector<feature*> * patients = (vector<feature*> *)(instanceArgs->get_vector(ARG_ACTION_PATIENT));
            
            vector<feature*>::iterator it = patients->begin();
            for(; it != patients->end(); ++it) {
            
                ok = agent->open_feature(*it);
                if (ok) {
                    response->successes->push_back((void*)(*it));
                } else {
                    response->errors->push_back(new error(ERR_GENERIC));
                    // Should return a specific error code
                }
            }
        }
        break;
        
        case EFF_FEAT_CLOSE:
        {
            actor * agent = (actor *)instanceArgs->get_from_vector(ARG_ACTION_AGENT);
            vector<feature*> * patients = (vector<feature*> *)(instanceArgs->get_vector(ARG_ACTION_PATIENT));
            
            vector<feature*>::iterator it = patients->begin();
            for(; it != patients->end(); ++it) {
            
                ok = agent->close_feature(*it);
                if (ok) {
                    response->successes->push_back((void*)(*it));
                } else {
                    response->errors->push_back(new error(ERR_GENERIC));
                    // Should return a specific error code
                }
            }
        }
        break;
        
        case EFF_STRIKE:
        {
            actor * agent = (actor *)instanceArgs->get_from_vector(ARG_ACTION_AGENT);
            vector<actor*> * patients = (vector<actor*>*)(instanceArgs->get_vector(ARG_ACTION_PATIENT));
            
            vector<actor*>::iterator it = patients->begin();
            for (; ok && it != patients->end(); ++it) {
            
                ok = agent->strike(*it);
                if (ok) {
                    response->successes->push_back((void*)(*it));
                } else {
                    response->errors->push_back(new error(ERR_GENERIC));
                    // Should return a specific error code
                }
            }
        }
        break;
        
        case EFF_PUNCH:
        {
            actor * agent = (actor *)instanceArgs->get_from_vector(ARG_ACTION_AGENT);
            vector<actor*> * patients = (vector<actor*>*)(instanceArgs->get_vector(ARG_ACTION_PATIENT));
            
            vector<actor*>::iterator it = patients->begin();
            for (; ok && it != patients->end(); ++it) {
            
                ok = agent->punch(*it);
                if (ok) {
                    response->successes->push_back((void*)(*it));
                } else {
                    response->errors->push_back(new error(ERR_GENERIC));
                    // Should return a specific error code
                }
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
        {
            actor * agent = (actor *)instanceArgs->get_from_vector(ARG_ACTION_AGENT);
            
			if(agent == act_player) {
				win_output->print("The bread is delicious!");
                //act->taste(TST_BREAD, true);
                //act->nourish(obj->bonus);
            }
        }
		break;
            
        case EFF_DRINK_WATER:
        {
            actor * agent = (actor *)instanceArgs->get_from_vector(ARG_ACTION_AGENT);
            if(agent == act_player) {
				win_output->print("The water is refreshing.");
            }
        }
		break;
            
		default:
			break;
            
        if(response->successes->size() > 0) {
            if (response->errors->size() == 0) {
                response->result = EFFRES_SUCCESS;
            } else {
                response->result = EFFRES_PARTIAL;
            }
        } else {
            response->result = EFFRES_FAILURE;
        }
            
        return response;
	}
}