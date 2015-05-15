#include "actor.h"
#include "argmap.h"
#include "error.h"
#include "mapentity.h"
#include "requirement.h"
#include "rogue.h"
#include "tile.h"
#include "window.h"

// Initializers and setup ====================================================

requirement::requirement(requirement_t ntype) {

    req_type = ntype;
    req_error_string = "";
    args = new argmap();
    args->add_int(ARG_REQUIRE_PARTIAL_TYPE, REQPAR_PARTIAL_OK);
}

requirement::requirement(string nerror, requirement_t ntype) {
    
    req_error_string = nerror;
    req_type = ntype;
    args = new argmap();
    args->add_int(ARG_REQUIRE_PARTIAL_TYPE, REQPAR_PARTIAL_OK);
}

void requirement::set_roles(args_t prim) {

    args->add_int(ARG_REQUIRE_PRIMARY, prim);
}

void requirement::set_roles(args_t prim, args_t sec) {

    args->add_int(ARG_REQUIRE_PRIMARY, prim);
    args->add_int(ARG_REQUIRE_SECONDARY, sec);
}

// ------------

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
        
    if (errors->size() > 0) {
        if (successes->size() > 0) {
            result = REQRES_PARTIAL;
        } else {
            result = REQRES_FAILURE;
        }        
    } else {
        result = REQRES_SUCCESS;
    }
}

// Evaluation of requiremetns ================================

require_resp * requirement::evaluate(requirement_t req_type, argmap * args, argmap * instanceArgs) {

    // false for OR, true for AND. If we ever see the opposite, we can return that.
    bool ok = true;
    bool req_all = args->get_int(ARG_REQUIRE_PARTIAL_TYPE) == REQPAR_PARTIAL_FAILS;
    bool negated = args->has_flag(FLAG_REQ_NEGATED);
    
    require_resp * response = new require_resp();
    
    switch(req_type) {
    
        case REQ_HAS_FLAG:
        {
            vector<entity*> * primary = (vector<entity*>*)get_primary(args, instanceArgs);
            if (primary && args->has_value(ARG_REQUIRE_FLAG)) {
            
                vector<entity*>::iterator it = primary->begin();
                for(; ((ok && req_all) || (!ok && !req_all)) && it != primary->end(); ++it) {
                
                    ok = (*it)->has_flag((flags_t)args->get_int(ARG_REQUIRE_FLAG));
                    if (ok) {
                        response->successes->push_back((void*)*it);
                    } else {
                        response->errors->push_back(new error(ERR_GENERIC));
                    }
                }
            } else {
                response->errors->push_back(new error(ERR_BAD_INPUT));
            }
        }
        break;
        
        case REQ_HAS_STAT:
        {
            vector<entity*> * primary = (vector<entity*>*)get_primary(args, instanceArgs);
            if (primary && args->has_value(ARG_REQUIRE_STAT)) {
            
                vector<entity*>::iterator it = primary->begin();
                for(; ((ok && req_all) || (!ok && !req_all)) && it != primary->end(); ++it) {
                
                    ok = (*it)->has_stat((stats_t)args->get_int(ARG_REQUIRE_STAT));
                    if (ok) {
                        response->successes->push_back((void*)*it);
                    } else {
                        response->errors->push_back(new error(ERR_GENERIC));
                    }
                }
            } else {
                response->errors->push_back(new error(ERR_BAD_INPUT));
            }
        }
        break;
        
        case REQ_STAT_EQUALS: // TODO - change this to general comparison later with an operator arg
        {
            vector<entity*> * primary = (vector<entity*>*)get_primary(args, instanceArgs);
            if (primary && args->has_value(ARG_REQUIRE_STAT)
             && args->has_value(ARG_REQUIRE_VALUE)) {
            
                vector<entity*>::iterator it = primary->begin();
                for(; ((ok && req_all) || (!ok && !req_all)) && it != primary->end(); ++it) {
                
                    ok = ( (*it)->has_stat((stats_t)args->get_int(ARG_REQUIRE_STAT)) &&
                        (*it)->get_stat((stats_t)args->get_int(ARG_REQUIRE_STAT)) == args->get_int(ARG_REQUIRE_VALUE));
                    if (ok) {
                        response->successes->push_back((void*)*it);
                    } else {
                        response->errors->push_back(new error(ERR_GENERIC));
                    }
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
            vector<entity*> * primary = (vector<entity*>*)get_primary(args, instanceArgs);
            if (primary->size() == 1) {
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
            tile * dest = ((vector<tile*>*)get_secondary(args, instanceArgs))->front();
            
            if (instanceArgs->has_value(ARG_ACTION_LOCATION)) {
                dest = ((vector<tile*>*)instanceArgs->get_vector(ARG_ACTION_LOCATION))->front();
            }
            if (dest == NULL) {
                ok = false;
                response->errors->push_back(new error(ERR_CANT_MOVE_TO));
                break;
            }
            
            // Are we capable of walking?
            //require_resp can_walk = requirement::evaluate(REQ_CAN_WALK, instanceArgs);
            requirement * can_walk = new requirement(REQ_ACTOR_CAN_WALK);
            can_walk->set_roles(ARG_ACTION_AGENT);
            if (can_walk->evaluate(instanceArgs)->result == REQRES_FAILURE) {
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
            actor * agent = ((vector<actor*>*)get_primary(args, instanceArgs))->front();
            if (agent == NULL) {
                response->errors->push_back(new error(ERR_BAD_INPUT));
                break;
            }
            if (agent->inventory->size() <= 0) {
                response->errors->push_back(new error(ERR_NO_ITEMS));
            }
        }
        break;
        
        case REQ_ACTOR_IS_PLAYER:
        {
            actor * agent = ((vector<actor*>*)get_primary(args, instanceArgs))->front();
            if (agent == NULL) {
                response->errors->push_back(new error(ERR_BAD_INPUT));
                break;
            }
            if (agent != act_player) {
                response->errors->push_back(new error(ERR_SILENT));
            }
        }
        break;
        
        // TODO - multitake.
        case REQ_ACTOR_CAN_TAKE:
        {
            actor * agent = ((vector<actor*>*)get_primary(args, instanceArgs))->front();
            vector<object*> * patients = (vector<object*>*)get_secondary(args, instanceArgs);
            if (agent != NULL && patients != NULL && patients->size() > 0) {
            
                for(int i = 0; ((ok && req_all) || (!ok && !req_all)) && i < patients->size(); ++i) {
                
                    if (!agent->can_take(patients->at(i))) {
                        response->errors->push_back(new error(ERR_FAIL));
                        ok = false;
                    }
                }
            } else {
                response->errors->push_back(new error(ERR_BAD_INPUT));
            }
        }
        break;
        
        case REQ_ACTOR_CAN_DROP:
        {
            actor * agent = ((vector<actor*>*)get_primary(args, instanceArgs))->front();
            vector<object*> * patients = (vector<object*>*)get_secondary(args, instanceArgs);
            if (agent != NULL && patients != NULL && patients->size() > 0) {

                for(int i = 0; ((ok && req_all) || (!ok && !req_all)) && i < patients->size(); ++i) {
                
                    if (!agent->can_drop(patients->at(i))) {
                        response->errors->push_back(new error(ERR_FAIL));
                        ok = false;
                    }
                }
            } else {
                response->errors->push_back(new error(ERR_BAD_INPUT));
            }
        }
        break;
        
        case REQ_ACTOR_CAN_EQUIP:
        {
            actor * agent = ((vector<actor*>*)get_primary(args, instanceArgs))->front();
            vector<object*> * patients = (vector<object*>*)get_secondary(args, instanceArgs);
            if (agent != NULL && patients != NULL && patients->size() > 0) {

                for(int i = 0; ((ok && req_all) || (!ok && !req_all)) && i < patients->size(); ++i) {
                
                    if (!agent->can_equip(patients->at(i))) {
                        response->errors->push_back(new error(ERR_FAIL));
                        ok = false;
                    }
                }
            } else {
                response->errors->push_back(new error(ERR_BAD_INPUT));
            } 
        }
        break;
        
        case REQ_ACTOR_CAN_UNEQUIP:
        {
            actor * agent = ((vector<actor*>*)get_primary(args, instanceArgs))->front();
            vector<object*> * patients = (vector<object*>*)get_secondary(args, instanceArgs);
            if (agent != NULL && patients != NULL && patients->size() > 0) {
            
                for(int i = 0; ((ok && req_all) || (!ok && !req_all)) && i < patients->size(); ++i) {
                    if (!agent->can_unequip(patients->at(i))) {
                        response->errors->push_back(new error(ERR_FAIL));
                        ok = false;
                    }
                }
            } else {
                response->errors->push_back(new error(ERR_BAD_INPUT));
            }
        }
        break;
        
        case REQ_ACTOR_CAN_EAT:
        {
            actor * agent = ((vector<actor*>*)get_primary(args, instanceArgs))->front();
            vector<object*> * patients = (vector<object*>*)get_secondary(args, instanceArgs);
            if (agent != NULL && patients != NULL && patients->size() > 0) {

                for(int i = 0; ((ok && req_all) || (!ok && !req_all)) && i < patients->size(); ++i) {
                    if (!agent->can_eat(patients->at(i))) {
                        response->errors->push_back(new error(ERR_FAIL));
                        ok = false;
                    }
                }
            } else {
                response->errors->push_back(new error(ERR_BAD_INPUT));
            }
        }
        break;
        
        case REQ_ACTOR_CAN_DRINK:
        {
            actor * agent = ((vector<actor*>*)get_primary(args, instanceArgs))->front();
            vector<object*> * patients = (vector<object*>*)get_secondary(args, instanceArgs);
            if (agent != NULL && patients != NULL && patients->size() > 0) {
            
                for(int i = 0; ((ok && req_all) || (!ok && !req_all)) && i < patients->size(); ++i) {
                    if (!agent->can_drink(patients->at(i))) {
                        response->errors->push_back(new error(ERR_FAIL));
                        ok = false;
                    }
                }
            } else {
                response->errors->push_back(new error(ERR_BAD_INPUT));
            }
        }
        break;
            
        case REQ_ACTOR_CAN_OPEN_FEAT:
        {
            actor * agent = ((vector<actor*>*)get_primary(args, instanceArgs))->front();
            vector<feature*> * patients = (vector<feature*>*)get_secondary(args, instanceArgs);
            if (agent != NULL && patients != NULL && patients->size() > 0) {
            
                for(int i = 0; ((ok && req_all) || (!ok && !req_all)) && i < patients->size(); ++i) {
                    if (agent->can_open(patients->at(i)) != ERR_NONE) {
                        response->errors->push_back(new error(ERR_FAIL));
                        ok = false;
                    }
                }
            } else {
                response->errors->push_back(new error(ERR_BAD_INPUT));
            }
        }
        break;
        
        case REQ_ACTOR_CAN_CLOSE_FEAT:
        {
            actor * agent = ((vector<actor*>*)get_primary(args, instanceArgs))->front();
            vector<feature*> * patients = (vector<feature*>*)get_secondary(args, instanceArgs);
            if (agent != NULL && patients != NULL && patients->size() > 0) {
            
                for(int i = 0; ((ok && req_all) || (!ok && !req_all)) && i < patients->size(); ++i) {
                    if (agent->can_close(patients->at(i)) != ERR_NONE) {
                        response->errors->push_back(new error(ERR_FAIL));
                        ok = false;
                    }
                }
            } else {
                response->errors->push_back(new error(ERR_BAD_INPUT));
            }
        }
        break;
        
        case REQ_ACTOR_CAN_STRIKE:
        {
            actor * agent = ((vector<actor*>*)get_primary(args, instanceArgs))->front();
            vector<actor*> * patients = (vector<actor*>*)get_secondary(args, instanceArgs);
            if (agent != NULL && patients != NULL && patients->size() > 0) {
            
                for(int i = 0; ((ok && req_all) || (!ok && !req_all)) && i < patients->size(); ++i) {
                    if (!agent->can_strike(patients->at(i))) {
                        response->errors->push_back(new error(ERR_FAIL));
                        ok = false;
                    }
                }
            } else {
                response->errors->push_back(new error(ERR_BAD_INPUT));
            }
        }
        break;
        
        case REQ_ACTOR_CAN_PUNCH:
        {
            actor * agent = ((vector<actor*>*)get_primary(args, instanceArgs))->front();
            vector<actor*> * patients = (vector<actor*>*)get_secondary(args, instanceArgs);
            if (agent != NULL && patients != NULL && patients->size() > 0) {
            
                for(int i = 0; ((ok && req_all) || (!ok && !req_all)) && i < patients->size(); ++i) {
                    if (!agent->can_punch(patients->at(i))) {
                        response->errors->push_back(new error(ERR_FAIL));
                        ok = false;
                    }
                }
            } else {
                response->errors->push_back(new error(ERR_BAD_INPUT));
            }
        }
        break;
                
        case REQ_NULL:
        default:
        break;
    }
    
    if (response->errors->size() > 0) {
        if (response->successes->size() > 0) {
            response->result = REQRES_PARTIAL;
        } else {
            response->result = REQRES_FAILURE;
        }        
    } else {
        response->result = REQRES_SUCCESS;
    }
    
    if (negated) {
        if (response->result == REQRES_FAILURE) {
            response->result = REQRES_SUCCESS;
        }
        else if(response->result == REQRES_SUCCESS) {
            response->result = REQRES_FAILURE;
        }
    }
    
    return response;
}

require_resp * requirement::evaluate(argmap * instanceArgs) {

    require_resp * response = requirement::evaluate(req_type, args, instanceArgs);
    
    if (response->result == REQRES_FAILURE && req_error_string != "") {
        win_output->print(req_error_string);
    }
    
    return response;
}

require_resp * requirement::evaluate_for(entity * ent, argmap * instanceArgs) {

    if (instanceArgs == NULL) {
        instanceArgs = new argmap();
    }

    instanceArgs->add_entity(ARG_ACTION_AGENT, ent);
    instanceArgs->add_int(ARG_REQUIRE_PRIMARY, (int)ARG_ACTION_AGENT);
    return evaluate(instanceArgs);
}

require_resp * requirement::evaluate_multiple(vector<requirement*> * reqs, argmap * instanceArgs) {

    require_resp * response = new require_resp();
    response->result = REQRES_SUCCESS;
            
    if (reqs == NULL) {

        return response;
    }

    vector<requirement*>::iterator it = reqs->begin();
    
    for(; it != reqs->end(); ++it) {
        
        require_resp * new_resp = (*it)->evaluate(instanceArgs);
        if (response == NULL) {
            response = new_resp;
        } else {
            response->merge(new_resp);
        }
    }

    return response;
}

require_resp * requirement::evaluate_multiple_for(vector<requirement*> * reqs, entity * ent, argmap * instanceArgs) {
  
    if (instanceArgs == NULL) {
        instanceArgs = new argmap();
    }
    
    instanceArgs->add_entity(ARG_ACTION_AGENT, ent);
    instanceArgs->add_int(ARG_REQUIRE_PRIMARY, (int)ARG_ACTION_AGENT);
    return evaluate_multiple(reqs, instanceArgs);
}

// HELPER FUNCTIONS ===========================================

vector<void*> * requirement::get_primary(argmap * args, argmap * instanceArgs){
    
    if (args->has_value(ARG_REQUIRE_PRIMARY)) {
    
        args_t val = (args_t)args->get_int(ARG_REQUIRE_PRIMARY);
        
        if (instanceArgs->has_value(val) ) {
            return instanceArgs->get_vector(val);
        }
    }
    
    return NULL;
}

vector<void*> * requirement::get_secondary(argmap * args, argmap * instanceArgs){
    
    if (args->has_value(ARG_REQUIRE_SECONDARY)) {
    
        args_t val = (args_t)args->get_int(ARG_REQUIRE_SECONDARY);
        
        if (instanceArgs->has_value(val)) {
            return instanceArgs->get_vector(val);
        }
    }
    
    return NULL;
}
