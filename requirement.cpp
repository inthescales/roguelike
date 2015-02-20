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
}

requirement::requirement(string nerror, requirement_t ntype) {
    
    error = nerror;
    req_type = ntype;
    negated = false;
    args = new argmap();
}

requirement::requirement(string nerror, requirement_t ntype, bool nneg) {
    
    error = nerror;
    req_type = ntype;
    negated = nneg;
    args = new argmap();
}

bool requirement::check() {

    return false;
}

// TODO - split this out of the requirement class so we can call
// "Process a requirement with index N and these args" without having to
// create a new requirement instance.
bool requirement::check_for(mapentity * ent) {

    bool ok = false;
    int err = 0;
    switch(req_type) {
    
        case REQ_HAS_FLAG:
            ok = (args->has_value(ARG_REQUIRE_FLAG) != 0 &&
                    ent->has_flag((flags_t)args->get_int(ARG_REQUIRE_FLAG)));
        break;
        case REQ_HAS_STAT:
            ok = (args->has_value(ARG_REQUIRE_STAT) != 0 &&
                    ent->has_stat((stats_t)args->get_int(ARG_REQUIRE_STAT)));
        break;
        case REQ_STAT_EQUALS:
            ok = (args->has_value(ARG_REQUIRE_STAT) != 0 &&
                    args->has_value(ARG_REQUIRE_VALUE) != 0 &&
                    ent->has_stat((stats_t)args->get_int(ARG_REQUIRE_STAT)) &&
                    ent->get_stat((stats_t)args->get_int(ARG_REQUIRE_STAT)) == args->get_int(ARG_REQUIRE_VALUE));
        break;
                
        case REQ_ACTOR_CAN_WALK:
            if (args->has_value(ARG_ACTION_PATIENT)) {
                vector<tile*> * patients = (vector<tile*> *)args->get_vector(ARG_ACTION_PATIENT);
                if (patients->size() > 0) {
                    err = ((actor *)ent)->can_walk(patients->at(0));
                    ok = (err == ERR_NONE);
                }
            }     
        break;
        
        case REQ_ACTOR_CAN_SWIM:
            if (args->has_value(ARG_ACTION_PATIENT)) {
                vector<tile*> * patients = (vector<tile*> *)args->get_vector(ARG_ACTION_PATIENT);
                if (patients->size() > 0) {
                    err = ((actor *)ent)->can_swim(patients->at(0));
                    ok = (err == ERR_NONE);
                }
            }        
        break;
        
        case REQ_ACTOR_CAN_FLY:
            if (args->has_value(ARG_ACTION_PATIENT)) {
                vector<tile*> * patients = (vector<tile*> *)args->get_vector(ARG_ACTION_PATIENT);
                if (patients->size() > 0) {
                    err = ((actor *)ent)->can_fly(patients->at(0));
                    ok = (err == ERR_NONE);
                }
            }
        break;
        
        case REQ_ACTOR_HAS_ITEMS:
            ok = ((actor *)ent)->inventory->size() > 0;
            if (!ok) err = ERR_NO_ITEMS;
        break;
        
        // TODO - multitake.
        case REQ_ACTOR_CAN_TAKE:
        
            if (args->has_value(ARG_ACTION_PATIENT)) {
                object * patient = args->get_object(ARG_ACTION_PATIENT);
                if (patient != NULL) {
                    err = ((actor *)ent)->can_take(patient);
                    ok = (err == ERR_NONE);
                } else {
                    ok = false;
                    err = ERR_NOTHING_TO_TAKE;
                }
            }     
        break;
        
        case REQ_ACTOR_CAN_DROP:
        
            if (args->has_value(ARG_ACTION_PATIENT)) {
                vector<object*> * patients = (vector<object*> *)args->get_vector(ARG_ACTION_PATIENT);
                if (patients->size() > 0) {
                    for(int i = 0; i < patients->size(); ++i) {
                        err = ((actor *)ent)->can_drop(patients->at(i));
                        ok = (err == ERR_NONE);
                        if (!ok) break;
                    }
                } else {
                    ok = false;
                    err = ERR_CANCELLED;
                }
            }     
        break;
        
        case REQ_ACTOR_CAN_EQUIP:
            
            if (args->has_value(ARG_ACTION_PATIENT)) {
                vector<object*> * patients = (vector<object*> *)args->get_vector(ARG_ACTION_PATIENT);
                if (patients->size() > 0) {
                    for(int i = 0; i < patients->size(); ++i) {
                        err = ((actor *)ent)->can_equip(patients->at(i));
                        ok = (err == ERR_NONE);
                        if (!ok) break;
                    }
                } else {
                    ok = false;
                    err = ERR_CANCELLED;
                }
            }
        break;
        
        case REQ_ACTOR_CAN_UNEQUIP:
            
            if (args->has_value(ARG_ACTION_PATIENT)) {
                vector<object*> * patients = (vector<object*> *)args->get_vector(ARG_ACTION_PATIENT);
                if (patients->size() > 0) {
                    for(int i = 0; i < patients->size(); ++i) {
                        err = ((actor *)ent)->can_unequip(patients->at(i));
                        ok = (err == ERR_NONE);
                        if (!ok) break;
                    }
                } else {
                    ok = false;
                    err = ERR_CANCELLED;
                }
            }
        break;
        
        case REQ_ACTOR_CAN_EAT:
        
            if (args->has_value(ARG_ACTION_PATIENT)) {
                vector<object*> * patients = (vector<object*> *)args->get_vector(ARG_ACTION_PATIENT);
                if (patients->size() > 0) {
                    for(int i = 0; i < patients->size(); ++i) {
                        err = ((actor *)ent)->can_eat(patients->at(i));
                        ok = (err == ERR_NONE);
                        if (!ok) break;
                    }
                } else {
                    ok = false;
                    err = ERR_CANCELLED;
                }
            }
        break;
        
        case REQ_ACTOR_CAN_DRINK:
        
            if (args->has_value(ARG_ACTION_PATIENT)) {
                vector<object*> * patients = (vector<object*> *)args->get_vector(ARG_ACTION_PATIENT);
                if (patients->size() > 0) {
                    for(int i = 0; i < patients->size(); ++i) {
                        err = ((actor *)ent)->can_drink(patients->at(i));
                        ok = (err == ERR_NONE);
                        if (!ok) break;
                    }
                } else {
                    ok = false;
                    err = ERR_CANCELLED;
                }
            }
        break;
            
        case REQ_ACTOR_CAN_OPEN_FEAT:
            
            if (args->has_value(ARG_ACTION_PATIENT)) {
                vector<feature*> * patients = (vector<feature*> *)args->get_vector(ARG_ACTION_PATIENT);
                if (patients->size() > 0){
                    for(int i = 0; i < patients->size(); ++i) {
                        err = ((actor *)ent)->can_open(patients->at(i));
                        ok = (err == ERR_NONE);
                        if (!ok) break;
                    }
                } else {
                    ok = false;
                    err = ERR_NOTHING_TO_OPEN;
                }
            }
        break;
        
        case REQ_ACTOR_CAN_CLOSE_FEAT:
            
            if (args->has_value(ARG_ACTION_PATIENT)) {
                vector<feature*> * patients = (vector<feature*> *)args->get_vector(ARG_ACTION_PATIENT);
                if (patients->size() > 0) {
                    for(int i = 0; i < patients->size(); ++i) {
                        err = ((actor *)ent)->can_close(patients->at(i));
                        ok = (err == ERR_NONE);
                        if (!ok) break;
                    }
                } else {
                    ok = false;
                    err = ERR_NOTHING_TO_CLOSE;
                }
            }
        break;
        
        case REQ_ACTOR_CAN_STRIKE:
            
            if (args->has_value(ARG_ACTION_PATIENT)) {
                vector<actor*> * patients = (vector<actor*> *)args->get_vector(ARG_ACTION_PATIENT);
                for(int i = 0; i < patients->size(); ++i) {
                    err = ((actor *)ent)->can_strike(patients->at(i));
                    ok = (err == ERR_NONE);
                    if (!ok) break;
                }
            }
        break;
        
        case REQ_ACTOR_CAN_PUNCH:
            
            if (args->has_value(ARG_ACTION_PATIENT)) {
                vector<actor*> * patients = (vector<actor*> *)args->get_vector(ARG_ACTION_PATIENT);
                for(int i = 0; i < patients->size(); ++i) {
                    ok = ((actor *)ent)->can_punch(patients->at(i));
                    ok = (err == ERR_NONE);
                    if (!ok) break;
                }
            }
        break;
        
        case REQ_NULL:
        default:
        break;
    }
    
    if (err != ERR_NONE) {
        ok = false;
        if (error != "") {
            win_output->print(error);
        } else if (err != ERR_SILENT) {
            win_output->print(error_string[err]);
        }
    }
    
    return ok;
}

bool requirement::check_requirements(vector<requirement*> * reqs) {

    vector<requirement*>::iterator it = reqs->begin();
    
    for(; it != reqs->end(); ++it) {
        
        if (!(*it)->check()) {
            return false;
        }
    }

    return true;
}

bool requirement::check_requirements_for(mapentity * ent, vector<requirement*> * reqs) {

    vector<requirement*>::iterator it = reqs->begin();
    
    for(; it != reqs->end(); ++it) {
        
        if (!(*it)->check_for(ent)) {
            return false;
        }
    }

    return true;
}
