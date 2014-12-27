#include "actor.h"
#include "argmap.h"
#include "mapentity.h"
#include "requirement.h"
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

bool requirement::check_for(mapentity * ent) {

    bool ok = false;
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
    
        case REQ_ACTOR_CAN_HOLD:
        
            if (args->has_value(ARG_ACTION_PATIENT)) {
                ok = ((actor *)ent)->can_take(args->get_object(ARG_ACTION_PATIENT));
            }     
            break;
            
        case REQ_ACTOR_CAN_EAT:
        
            if (args->has_value(ARG_ACTION_PATIENT)) {
                vector<object*> * patients = (vector<object*> *)args->get_vector(ARG_ACTION_PATIENT);
                for(int i = 0; i < patients->size(); ++i) {
                    ok = ((actor *)ent)->can_eat(patients->at(i));
                    if (!ok) break;
                }
            }
            break;
            
        case REQ_NULL:
        default:
            break;
    }
    
    if (!ok) {
        win_output->print(error);
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
