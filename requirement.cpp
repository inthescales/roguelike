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
    error = "";
    negated = false;
    args = new argmap();
    externalArgs = NULL;
}

requirement::requirement(string nerror, requirement_t ntype) {
    
    error = nerror;
    req_type = ntype;
    negated = false;
    args = new argmap();
    externalArgs = NULL;
}

requirement::requirement(string nerror, requirement_t ntype, bool nneg, bool nall) {
    
    error = nerror;
    req_type = ntype;
    negated = nneg;
    req_all = nall;
    args = new argmap();
    externalArgs = NULL;
}

// TODO - split this out of the requirement class so we can call
// "Process a requirement with index N and these args" without having to
// create a new requirement instance.

set<error_t> * requirement::check() {

    // false for OR, true for AND. If we ever see the opposite, we can return that.
    bool ok = true; 
    set<error_t> * errs = new set<error_t>();
    
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
                    errs->insert(ERR_SILENT);
                }
            } else {
                errs->insert(ERR_BAD_INPUT);
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
                    errs->insert(ERR_SILENT);
                }
            } else {
                errs->insert(ERR_BAD_INPUT);
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
                    errs->insert(ERR_SILENT);
                }
            } else {
                errs->insert(ERR_BAD_INPUT);
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
                errs->insert(ERR_CANT_WALK);
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
                errs->insert(ERR_CANT_MOVE_TO);
                break;
            }
            
            // Are we capable of walking?
            requirement * can_walk = new requirement(REQ_ACTOR_CAN_WALK);
            can_walk->args->add_int(ARG_REQUIRE_UNARY_ROLE, ARG_ACTION_AGENT);
            can_walk->args->add_actor(ARG_ACTION_AGENT, (actor*)get_used_arg(ARG_ACTION_AGENT));
            if (can_walk->check() != NULL) {
                ok = false;
                errs->insert(ERR_CANT_WALK);
                break;
            }
            
            // Is the destination a valid place for us to walk to?
            if (dest == NULL
             || !dest->has_flag(FLAG_TILE_CAN_WALK)
             || dest->my_feature != NULL
             || dest->my_actor != NULL) {
                ok = false;
                errs->insert(ERR_CANT_MOVE_TO);
            }
        }
        break;
        
        case REQ_ACTOR_HAS_ITEMS:
        {
            vector<void*> * unary = get_unary();
            if (unary == NULL || unary->size() == 0) {
                errs->insert(ERR_BAD_INPUT);
                break;
            }
            if (((actor*)unary->front())->inventory->size() <= 0) {
                errs->insert(ERR_NO_ITEMS);
            }
        }
        break;
        
        case REQ_ACTOR_IS_PLAYER:
        {
            vector<void*> * unary = get_unary();
            if (unary == NULL || unary->size() == 0) {
                errs->insert(ERR_BAD_INPUT);
                break;
            }
            if (unary->front() != act_player) {
                errs->insert(ERR_SILENT);
            }
        }
        break;
        
        // TODO - multitake.
        case REQ_ACTOR_CAN_TAKE:
        
            if (has_used_arg(ARG_ACTION_PATIENT)) {
                object * patient = (object*)get_used_arg(ARG_ACTION_PATIENT);
                if (patient != NULL) {
                    if (!((actor*)get_used_arg(ARG_ACTION_AGENT))->can_take(patient)) {
                        errs->insert(ERR_FAIL);
                    }
                } else {
                    errs->insert(ERR_NOTHING_TO_TAKE);
                }
            } else {
                errs->insert(ERR_BAD_INPUT);
            } 
        break;
        
        case REQ_ACTOR_CAN_DROP:
        
            if ((actor*)get_used_arg(ARG_ACTION_AGENT) && has_used_arg(ARG_ACTION_PATIENT)) {
                vector<object*> * patients = (vector<object*> *)get_used_arg(ARG_ACTION_PATIENT);
                if (patients->size() > 0) {
                    for(int i = 0; ok && i < patients->size(); ++i) {
                        if (!((actor*)get_used_arg(ARG_ACTION_AGENT))->can_drop(patients->at(i))) {
                            errs->insert(ERR_FAIL);
                            ok = false;
                        }
                    }
                } else {
                    errs->insert(ERR_CANCELLED);
                }
            } else {
                errs->insert(ERR_BAD_INPUT);
            } 
        break;
        
        case REQ_ACTOR_CAN_EQUIP:
            
            if ((actor*)get_used_arg(ARG_ACTION_AGENT) && has_used_arg(ARG_ACTION_PATIENT)) {
                vector<object*> * patients = (vector<object*> *)get_used_arg(ARG_ACTION_PATIENT);
                if (patients->size() > 0) {
                    for(int i = 0; ok && i < patients->size(); ++i) {
                        if ( !((actor*)get_used_arg(ARG_ACTION_AGENT))->can_equip(patients->at(i)) ) {
                            errs->insert(ERR_FAIL);
                            ok = false;
                        }
                    }
                } else {
                    errs->insert(ERR_CANCELLED);
                }
            } else {
                errs->insert(ERR_BAD_INPUT);
            } 
        break;
        
        case REQ_ACTOR_CAN_UNEQUIP:
            
            if ((actor*)get_used_arg(ARG_ACTION_AGENT) && has_used_arg(ARG_ACTION_PATIENT)) {
                vector<object*> * patients = (vector<object*> *)get_used_arg(ARG_ACTION_PATIENT);
                if (patients->size() > 0) {
                    for(int i = 0; ok && i < patients->size(); ++i) {
                        if ( !((actor*)get_used_arg(ARG_ACTION_AGENT))->can_unequip(patients->at(i)) ) {
                            errs->insert(ERR_FAIL);
                            ok = false;
                        }
                    }
                } else {
                    errs->insert(ERR_CANCELLED);
                }
            } else {
                errs->insert(ERR_BAD_INPUT);
            } 
        break;
        
        case REQ_ACTOR_CAN_EAT:
        
            if ((actor*)get_used_arg(ARG_ACTION_AGENT) && has_used_arg(ARG_ACTION_PATIENT)) {
                vector<object*> * patients = (vector<object*> *)get_used_arg(ARG_ACTION_PATIENT);
                if (patients->size() > 0) {
                    for(int i = 0; ok && i < patients->size(); ++i) {
                        if ( !((actor*)get_used_arg(ARG_ACTION_AGENT))->can_eat(patients->at(i)) ) {
                            errs->insert(ERR_FAIL);
                            ok = false;
                        }
                    }
                } else {
                    errs->insert(ERR_CANCELLED);
                }
            } else {
                errs->insert(ERR_BAD_INPUT);
            } 
        break;
        
        case REQ_ACTOR_CAN_DRINK:
        
            if ((actor*)get_used_arg(ARG_ACTION_AGENT) && has_used_arg(ARG_ACTION_PATIENT)) {
                vector<object*> * patients = (vector<object*> *)get_used_arg(ARG_ACTION_PATIENT);
                if (patients->size() > 0) {
                    for(int i = 0; ok && i < patients->size(); ++i) {
                        if ( !((actor*)get_used_arg(ARG_ACTION_AGENT))->can_drink(patients->at(i)) ) {
                            errs->insert(ERR_FAIL);
                            ok = false;
                        }
                    }
                } else {
                    errs->insert(ERR_CANCELLED);
                }
            } else {
                errs->insert(ERR_BAD_INPUT);
            } 
        break;
            
        case REQ_ACTOR_CAN_OPEN_FEAT:
            
            if ((actor*)get_used_arg(ARG_ACTION_AGENT) && has_used_arg(ARG_ACTION_PATIENT)) {
                vector<feature*> * patients = (vector<feature*> *)get_used_arg(ARG_ACTION_PATIENT);
                if (patients->size() > 0){
                    for(int i = 0; ok && i < patients->size(); ++i) {
                        if ( !((actor*)get_used_arg(ARG_ACTION_AGENT))->can_open(patients->at(i)) ) {
                            errs->insert(ERR_FAIL);
                            ok = false;
                        }
                    }
                } else {
                    errs->insert(ERR_NOTHING_TO_OPEN);
                }
            } else {
                errs->insert(ERR_BAD_INPUT);
            } 
        break;
        
        case REQ_ACTOR_CAN_CLOSE_FEAT:
            
            if ((actor*)get_used_arg(ARG_ACTION_AGENT) && has_used_arg(ARG_ACTION_PATIENT)) {
                vector<feature*> * patients = (vector<feature*> *)get_used_arg(ARG_ACTION_PATIENT);
                if (patients->size() > 0) {
                    for(int i = 0; ok && i < patients->size(); ++i) {
                        if ( !((actor*)get_used_arg(ARG_ACTION_AGENT))->can_close(patients->at(i)) ) {
                            errs->insert(ERR_FAIL);
                            ok = false;
                        }
                    }
                } else {
                    errs->insert(ERR_NOTHING_TO_CLOSE);
                }
            } else {
                errs->insert(ERR_BAD_INPUT);
            } 
        break;
        
        case REQ_ACTOR_CAN_STRIKE:
            
            if ((actor*)get_used_arg(ARG_ACTION_AGENT) && has_used_arg(ARG_ACTION_PATIENT)) {
                vector<actor*> * patients = (vector<actor*> *)get_used_arg(ARG_ACTION_PATIENT);
                for(int i = 0; ok && i < patients->size(); ++i) {
                    if ( !((actor*)get_used_arg(ARG_ACTION_AGENT))->can_strike(patients->at(i)) ) {
                        errs->insert(ERR_FAIL);
                        ok = false;
                    }
                }
            } else {
                errs->insert(ERR_BAD_INPUT);
            } 
        break;
        
        case REQ_ACTOR_CAN_PUNCH:
            
            if ((actor*)get_used_arg(ARG_ACTION_AGENT) && has_used_arg(ARG_ACTION_PATIENT)) {
                vector<actor*> * patients = (vector<actor*> *)get_used_arg(ARG_ACTION_PATIENT);
                for(int i = 0; ok &&i < patients->size(); ++i) {
                    if ( !((actor*)get_used_arg(ARG_ACTION_AGENT))->can_punch(patients->at(i)) ) {
                        errs->insert(ERR_FAIL);
                        ok = false;
                    }
                }
            } else {
                errs->insert(ERR_BAD_INPUT);
            } 
        break;
                
        case REQ_NULL:
        default:
        break;
    }
    
    if (errs->size() > 0) {
        if (error != "") {
            win_output->print(error);
        } else {
            int highest = 0; /* - get_highest_error(errs);*/
            win_output->print(error_string[highest]);
        }
    }
    
    clear_external_args();
    
    if((errs->size() == 0 && !negated) || (errs->size() > 0 && negated)) {
        return NULL;
    } else {
        return errs;
    }
}

set<error_t> * requirement::check_for(entity * ent) {

    args->add_entity(ARG_ACTION_AGENT, ent);
    args->add_int(ARG_REQUIRE_UNARY_ROLE, (int)ACTROLE_AGENT);
    return check();
}

set<error_t> * requirement::check_requirements(vector<requirement*> * reqs, argmap * externalArgs) {

    if (reqs == NULL) {
        return NULL;
    }
    
    set<error_t> * errs = new set<error_t>();
    vector<requirement*>::iterator it = reqs->begin();
    
    for(; it != reqs->end(); ++it) {
        
        (*it)->externalArgs = externalArgs;
        set<error_t> * new_errs = (*it)->check();
        if (new_errs != NULL) {
            errs->insert(new_errs->begin(), new_errs->end());
        }
    }

    int test = errs->size();
    
    if (errs->size() == 0) {
        return NULL;
    } else {
        return errs;
    }
}

set<error_t> * requirement::check_requirements_for(vector<requirement*> * reqs, entity * ent, argmap * externalArgs) {

    if (reqs == NULL) {
        return NULL;
    }
    
    set<error_t> * errs = new set<error_t>();
    vector<requirement*>::iterator it = reqs->begin();
    
    for(; it != reqs->end(); ++it) {
        
        (*it)->externalArgs = externalArgs;
        set<error_t> * new_errs = (*it)->check_for(ent);
        if (new_errs != NULL) {
            errs->insert(new_errs->begin(), new_errs->end());
        }
    }
    
    if (errs->size() == 0) {
        return NULL;
    } else {
        return errs;
    }
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