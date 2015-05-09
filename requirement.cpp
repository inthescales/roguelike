#include "actor.h"
#include "argmap.h"
#include "error.h"
#include "mapentity.h"
#include "requirement.h"
#include "rogue.h"
#include "tile.h"
#include "window.h"

requirement::requirement(requirement_t ntype) {

    req_type = ntype;
    req_error_string = "";
    negated = false;
    args = new argmap();
    externalArgs = NULL;
}

requirement::requirement(string nerror, requirement_t ntype) {
    
    req_error_string = nerror;
    req_type = ntype;
    negated = false;
    args = new argmap();
    externalArgs = NULL;
}

requirement::requirement(string nerror, requirement_t ntype, bool nneg, bool nall) {
    
    req_error_string = nerror;
    req_type = ntype;
    negated = nneg;
    req_all = nall;
    args = new argmap();
    externalArgs = NULL;
}

require_resp::require_resp() {
    
    result = REQRES_UNKNOWN;
    successes = new vector<void*>();
    errors = new vector<error*>();
}

/*
    Merge two require responses.
    Result has intersection of successes, union of errors.
    State is set accordingly.
*/
void require_resp::merge(require_resp * other) {

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
}

// TODO - split this out of the requirement class so we can call
// "Process a requirement with index N and these args" without having to
// create a new requirement instance.

require_resp * requirement::check() {

    // false for OR, true for AND. If we ever see the opposite, we can return that.
    bool ok = true;
    
    require_resp * response = new require_resp();
    
    switch(req_type) {
    
        case REQ_HAS_FLAG:
        {
            vector<void*> * unary = get_unary();
            if (unary && args->has_value(ARG_REQUIRE_FLAG)) {
                vector<void*>::iterator it = unary->begin();
                for(; ((ok && req_all) || (!ok && !req_all)) && it != unary->end(); ++it) {
                    int testFlag = args->get_int(ARG_REQUIRE_FLAG);
                    bool isitok = ((entity*)(*it))->has_flag((flags_t)testFlag);
                    tile * test = (tile*)(*it);
                    ok = ((entity*)(*it))->has_flag((flags_t)args->get_int(ARG_REQUIRE_FLAG));
                }
                if (!ok) {
                    response->errors->push_back(new error(ERR_SILENT));
                }
            } else {
                response->errors->push_back(new error(ERR_BAD_INPUT));
            }
        }
        break;
        case REQ_HAS_STAT:
        {
            vector<void*> * unary = get_unary();
            if (unary && args->has_value(ARG_REQUIRE_STAT)) {
                vector<void*>::iterator it = unary->begin();
                for(; ((ok && req_all) || (!ok && !req_all)) && it != unary->end(); ++it) {
                    ok = ((entity*)(*it))->has_stat((stats_t)args->get_int(ARG_REQUIRE_STAT));
                }
                if (!ok) {
                    response->errors->push_back(new error(ERR_SILENT));
                }
            } else {
                response->errors->push_back(new error(ERR_BAD_INPUT));
            }
        }
        break;
        case REQ_STAT_EQUALS:
        {
            vector<void*> * unary = get_unary();
            if (unary && args->has_value(ARG_REQUIRE_STAT)
             && args->has_value(ARG_REQUIRE_VALUE)) {
            
                vector<void*>::iterator it = unary->begin();
                for(; ((ok && req_all) || (!ok && !req_all)) && it != unary->end(); ++it) {
                    if((((entity*)(*it))->has_stat((stats_t)args->get_int(ARG_REQUIRE_STAT)) &&
                        ((entity*)(*it))->get_stat((stats_t)args->get_int(ARG_REQUIRE_STAT)) == args->get_int(ARG_REQUIRE_VALUE)) != ok) {
                        ok = true;
                    } else ok = false;
                }
                if (!ok) {
                    response->errors->push_back(new error(ERR_SILENT));
                }
            } else {
                response->errors->push_back(new error(ERR_BAD_INPUT));
            }
        }
        break;
        
        // ============================================================
        
        case REQ_ACTOR_CAN_WALK:
        {
            // An actor with a walk action is considered theoretically capable of walking.
            // This checks that it is at this moment able to walk.
            vector<void*> * unary = get_unary();
            if (unary->size() == 1) {
                ok = true;
            } else {
                ok = false;
                response->errors->push_back(new error(ERR_CANT_WALK));
            }
        }
        break;
        
        case REQ_ACTOR_CAN_WALK_TO:
        {
            // Do we have a valid destination?
            tile * dest = NULL;
            if (has_used_arg(ARG_ACTION_LOCATION)) {
                dest = ((vector<tile*>*)get_used_arg(ARG_ACTION_LOCATION))->front();
            }
            if (dest == NULL) {
                ok = false;
                response->errors->push_back(new error(ERR_CANT_MOVE_TO));
                break;
            }
            
            // Are we capable of walking?
            requirement * can_walk = new requirement(REQ_ACTOR_CAN_WALK);
            can_walk->args->add_int(ARG_REQUIRE_UNARY_ROLE, ARG_ACTION_AGENT);
            can_walk->args->add_actor(ARG_ACTION_AGENT, (actor*)get_used_arg(ARG_ACTION_AGENT));
            if (can_walk->check()->result == REQRES_FAILURE) {
                ok = false;
                response->errors->push_back(new error(ERR_CANT_WALK));
                break;
            }
            
            // Is the destination a valid place for us to walk to?
            if (dest == NULL
             || !dest->has_flag(FLAG_TILE_CAN_WALK)
             || dest->my_feature != NULL
             || dest->my_actor != NULL) {
                ok = false;
                response->errors->push_back(new error(ERR_CANT_MOVE_TO));
            }
        }
        break;
        
        case REQ_ACTOR_HAS_ITEMS:
        {
            vector<void*> * unary = get_unary();
            if (unary == NULL || unary->size() == 0) {
                response->errors->push_back(new error(ERR_BAD_INPUT));
                break;
            }
            if (((actor*)unary->front())->inventory->size() <= 0) {
                response->errors->push_back(new error(ERR_NO_ITEMS));
            }
        }
        break;
        
        case REQ_ACTOR_IS_PLAYER:
        {
            vector<void*> * unary = get_unary();
            if (unary == NULL || unary->size() == 0) {
                response->errors->push_back(new error(ERR_BAD_INPUT));
                break;
            }
            if (unary->front() != act_player) {
                response->errors->push_back(new error(ERR_SILENT));
            }
        }
        break;
        
        // TODO - multitake.
        case REQ_ACTOR_CAN_TAKE:
        
            if (has_used_arg(ARG_ACTION_PATIENT)) {
                object * patient = (object*)get_used_arg(ARG_ACTION_PATIENT);
                if (patient != NULL) {
                    if (((actor*)get_used_arg(ARG_ACTION_AGENT))->can_take(patient) != ERR_NONE) {
                        response->errors->push_back(new error(ERR_FAIL));
                    }
                } else {
                    response->errors->push_back(new error(ERR_NOTHING_TO_TAKE));
                }
            } else {
                response->errors->push_back(new error(ERR_BAD_INPUT));
            } 
        break;
        
        case REQ_ACTOR_CAN_DROP:
        
            if ((actor*)get_used_arg(ARG_ACTION_AGENT) && has_used_arg(ARG_ACTION_PATIENT)) {
                vector<object*> * patients = (vector<object*> *)get_used_arg(ARG_ACTION_PATIENT);
                if (patients->size() > 0) {
                    for(int i = 0; ok && i < patients->size(); ++i) {
                        if (!((actor*)get_used_arg(ARG_ACTION_AGENT))->can_drop(patients->at(i))) {
                            response->errors->push_back(new error(ERR_FAIL));
                            ok = false;
                        }
                    }
                } else {
                    response->errors->push_back(new error(ERR_CANCELLED));
                }
            } else {
                response->errors->push_back(new error(ERR_BAD_INPUT));
            } 
        break;
        
        case REQ_ACTOR_CAN_EQUIP:
            
            if ((actor*)get_used_arg(ARG_ACTION_AGENT) && has_used_arg(ARG_ACTION_PATIENT)) {
                vector<object*> * patients = (vector<object*> *)get_used_arg(ARG_ACTION_PATIENT);
                if (patients->size() > 0) {
                    for(int i = 0; ok && i < patients->size(); ++i) {
                        if ( !((actor*)get_used_arg(ARG_ACTION_AGENT))->can_equip(patients->at(i)) ) {
                            response->errors->push_back(new error(ERR_FAIL));
                            ok = false;
                        }
                    }
                } else {
                    response->errors->push_back(new error(ERR_CANCELLED));
                }
            } else {
                response->errors->push_back(new error(ERR_BAD_INPUT));
            } 
        break;
        
        case REQ_ACTOR_CAN_UNEQUIP:
            
            if ((actor*)get_used_arg(ARG_ACTION_AGENT) && has_used_arg(ARG_ACTION_PATIENT)) {
                vector<object*> * patients = (vector<object*> *)get_used_arg(ARG_ACTION_PATIENT);
                if (patients->size() > 0) {
                    for(int i = 0; ok && i < patients->size(); ++i) {
                        if ( !((actor*)get_used_arg(ARG_ACTION_AGENT))->can_unequip(patients->at(i)) ) {
                            response->errors->push_back(new error(ERR_FAIL));
                            ok = false;
                        }
                    }
                } else {
                    response->errors->push_back(new error(ERR_CANCELLED));
                }
            } else {
                response->errors->push_back(new error(ERR_BAD_INPUT));
            } 
        break;
        
        case REQ_ACTOR_CAN_EAT:
        
            if ((actor*)get_used_arg(ARG_ACTION_AGENT) && has_used_arg(ARG_ACTION_PATIENT)) {
                vector<object*> * patients = (vector<object*> *)get_used_arg(ARG_ACTION_PATIENT);
                if (patients->size() > 0) {
                    for(int i = 0; ok && i < patients->size(); ++i) {
                        if ( !((actor*)get_used_arg(ARG_ACTION_AGENT))->can_eat(patients->at(i)) ) {
                            response->errors->push_back(new error(ERR_FAIL));
                            ok = false;
                        }
                    }
                } else {
                    response->errors->push_back(new error(ERR_CANCELLED));
                }
            } else {
                response->errors->push_back(new error(ERR_BAD_INPUT));
            } 
        break;
        
        case REQ_ACTOR_CAN_DRINK:
        
            if ((actor*)get_used_arg(ARG_ACTION_AGENT) && has_used_arg(ARG_ACTION_PATIENT)) {
                vector<object*> * patients = (vector<object*> *)get_used_arg(ARG_ACTION_PATIENT);
                if (patients->size() > 0) {
                    for(int i = 0; ok && i < patients->size(); ++i) {
                        if ( !((actor*)get_used_arg(ARG_ACTION_AGENT))->can_drink(patients->at(i)) ) {
                            response->errors->push_back(new error(ERR_FAIL));
                            ok = false;
                        }
                    }
                } else {
                    response->errors->push_back(new error(ERR_CANCELLED));
                }
            } else {
                response->errors->push_back(new error(ERR_BAD_INPUT));
            } 
        break;
            
        case REQ_ACTOR_CAN_OPEN_FEAT:
            
            if ((actor*)get_used_arg(ARG_ACTION_AGENT) && has_used_arg(ARG_ACTION_PATIENT)) {
                vector<feature*> * patients = (vector<feature*> *)get_used_arg(ARG_ACTION_PATIENT);
                if (patients->size() > 0){
                    for(int i = 0; ok && i < patients->size(); ++i) {
                        if ( ((actor*)get_used_arg(ARG_ACTION_AGENT))->can_open(patients->at(i)) != ERR_NONE) {
                            response->errors->push_back(new error(ERR_FAIL));
                            ok = false;
                        }
                    }
                } else {
                    response->errors->push_back(new error(ERR_NOTHING_TO_OPEN));
                }
            } else {
                response->errors->push_back(new error(ERR_BAD_INPUT));
            } 
        break;
        
        case REQ_ACTOR_CAN_CLOSE_FEAT:
            
            if ((actor*)get_used_arg(ARG_ACTION_AGENT) && has_used_arg(ARG_ACTION_PATIENT)) {
                vector<feature*> * patients = (vector<feature*> *)get_used_arg(ARG_ACTION_PATIENT);
                if (patients->size() > 0) {
                    for(int i = 0; ok && i < patients->size(); ++i) {
                        if ( !((actor*)get_used_arg(ARG_ACTION_AGENT))->can_close(patients->at(i))  != ERR_NONE) {
                            response->errors->push_back(new error(ERR_FAIL));
                            ok = false;
                        }
                    }
                } else {
                    response->errors->push_back(new error(ERR_NOTHING_TO_CLOSE));
                }
            } else {
                response->errors->push_back(new error(ERR_BAD_INPUT));
            } 
        break;
        
        case REQ_ACTOR_CAN_STRIKE:
            
            if ((actor*)get_used_arg(ARG_ACTION_AGENT) && has_used_arg(ARG_ACTION_PATIENT)) {
                vector<actor*> * patients = (vector<actor*> *)get_used_arg(ARG_ACTION_PATIENT);
                for(int i = 0; ok && i < patients->size(); ++i) {
                    if ( !((actor*)get_used_arg(ARG_ACTION_AGENT))->can_strike(patients->at(i)) ) {
                        response->errors->push_back(new error(ERR_FAIL));
                        ok = false;
                    }
                }
            } else {
                response->errors->push_back(new error(ERR_BAD_INPUT));
            } 
        break;
        
        case REQ_ACTOR_CAN_PUNCH:
            
            if ((actor*)get_used_arg(ARG_ACTION_AGENT) && has_used_arg(ARG_ACTION_PATIENT)) {
                vector<actor*> * patients = (vector<actor*> *)get_used_arg(ARG_ACTION_PATIENT);
                for(int i = 0; ok &&i < patients->size(); ++i) {
                    if ( !((actor*)get_used_arg(ARG_ACTION_AGENT))->can_punch(patients->at(i)) ) {
                        response->errors->push_back(new error(ERR_FAIL));
                        ok = false;
                    }
                }
            } else {
                response->errors->push_back(new error(ERR_BAD_INPUT));
            } 
        break;
                
        case REQ_NULL:
        default:
        break;
    }
    
    if (response->errors->size() > 0) {
    
        response->result = REQRES_FAILURE;
    
        if (req_error_string != "") {
            win_output->print(req_error_string);
        } else {
            int highest = 0; /* - get_highest_error(response->errors);*/
            win_output->print(error_string[highest]);
        }
    } else {
        response->result = REQRES_SUCCESS;
    }
    
    clear_external_args();
    
    return response;
}

require_resp * requirement::check_for(entity * ent) {

    args->add_entity(ARG_ACTION_AGENT, ent);
    args->add_int(ARG_REQUIRE_UNARY_ROLE, (int)ACTROLE_AGENT);
    return check();
}

require_resp * requirement::check_requirements(vector<requirement*> * reqs, argmap * externalArgs) {

    require_resp * response = new require_resp();
    response->result = REQRES_SUCCESS;
            
    if (reqs == NULL) {

        return response;
    }

    vector<requirement*>::iterator it = reqs->begin();
    
    for(; it != reqs->end(); ++it) {
        
        (*it)->externalArgs = externalArgs;
        require_resp * new_resp = (*it)->check();
        if (response == NULL) {
            response = new_resp;
        } else {
            response->merge(new_resp);
        }
    }

    return response;
}

require_resp * requirement::check_requirements_for(vector<requirement*> * reqs, entity * ent, argmap * externalArgs) {
  
    externalArgs->add_entity(ARG_ACTION_AGENT, ent);
    externalArgs->add_int(ARG_REQUIRE_UNARY_ROLE, (int)ACTROLE_AGENT);
    return check_requirements(reqs, externalArgs);
}


// HELPER FUNCTIONS ===========================================

void requirement::clear_external_args() {
    externalArgs = NULL;
}

void * requirement::get_used_arg(args_t code) {

    if (externalArgs != NULL && externalArgs->has_value(code)) {
        return (void*)externalArgs->get_int(code);
    } else {
        return (void*)args->get_int(code);
    }
}

bool requirement::has_used_arg(args_t arg) {
    
    return (args->has_value(arg) || externalArgs->has_value(arg));
}

vector<void*> * requirement::get_unary() {

    vector<void*> * ret = NULL;

    if (has_used_arg((args_t)ARG_REQUIRE_UNARY_ROLE)) {
        
        if ((int)get_used_arg(ARG_REQUIRE_UNARY_ROLE) == ARG_ACTION_AGENT) {
            ret = new vector<void*>();
            ret->push_back((void*)get_used_arg(ARG_ACTION_AGENT));
        } else {
            ret = (vector<void*>*)get_used_arg((args_t)externalArgs->get_int((args_t)ARG_REQUIRE_UNARY_ROLE));
        }
    }
    
    return ret;
}