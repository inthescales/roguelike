#include "actor.h"
#include "AI.h"
#include "classdefs.h"
#include "feature.h"
#include "object.h"
#include "tile.h"
#include "vision.h"

#include <set>

using std::set;

aimodule * aimod[AI_MAX];

motivator::motivator(bool s, requirement * req2, int val) {
    
    self = s;
    req = req2;
    value = val;
}

idea::idea(entity_type etype, actionPurpose_t purp, int b) {

    e_type = etype;
    purpose = purp;
    base = b;
    motivators = new vector<motivator*>();
}

aimodule::aimodule() {

    ideas = new std::map<entity_type, vector<idea*>* >();
}

void aimodule::add_idea(idea * id) {

    if ((*ideas)[id->e_type] == NULL) {
        (*ideas)[id->e_type] = new vector<idea*>();
    }
    
    (*ideas)[id->e_type]->push_back(id);
}

mind::mind(aimodule * mod) {

    module = mod;
    cur_goal = NULL;
}

goal::goal(entity * targ, actionPurpose_t purp) {

    target = targ;
    purpose = purp;
}

/*
  Not in use because I don't have a good way to return
  all the things. I guess I could have sense return a map
  from entity type to a vector of entities but I'll think
  about that later.
*/
vector<entity*> * AI::sense_entities(actor * act) {
    
    return NULL;
}

goal * AI::select_goal(actor * decider) {

    // Find entities
    set<tile*> * tiles = vision::tiles_in_view(decider);
    set<actor*> * actors = new set<actor*>();
    set<object*> * objects = new set<object*>();
    set<feature*> * features = new set<feature*>();
    
    set<tile*>::iterator it = tiles->begin();
    for(; it != tiles->end(); ++it) {
        
        if((*it)->my_actor != NULL) {
            actors->insert((*it)->my_actor);
        }
        if((*it)->my_objects != NULL) {
            vector<object*>::iterator obj_it = (*it)->my_objects->begin();
            for(;obj_it != (*it)->my_objects->end(); ++obj_it) {
                objects->insert(*obj_it);
            }
        }
        if((*it)->my_feature != NULL) {
            features->insert((*it)->my_feature);
        }
    }
    
    vector<idea*> * ideas;
    int best_val = -1000;
    entity * best_target = NULL;
    actionPurpose_t best_purpose = ACTPUR_NONE;
    vector<idea*>::iterator idea_it;
    
    ideas = (*decider->my_mind->module->ideas)[ENT_TYPE_ACTOR];
    if(ideas != NULL) {
        set<actor*>::iterator act_it = actors->begin();
        for(; act_it != actors->end(); ++act_it) {
            
            idea_it = ideas->begin();
            for(; idea_it != ideas->end(); ++idea_it) {
            
                int val = idea_value(decider, (*act_it), (*idea_it));
                if (val > best_val && decider->get_actions_for((*idea_it)->purpose)->size() > 0) {
                    best_val = val;
                    best_target = (*act_it);
                    best_purpose = (*idea_it)->purpose;
                }
            }
        }
    }
    
    ideas = (*decider->my_mind->module->ideas)[ENT_TYPE_OBJECT];    
    if(ideas != NULL) {
        set<object*>::iterator obj_it = objects->begin();
        for(; obj_it != objects->end(); ++obj_it) {
            
            idea_it = ideas->begin();
            for(; idea_it != ideas->end(); ++idea_it) {
                int val = idea_value(decider, (entity*)(*obj_it), (*idea_it));
                if (val > best_val && decider->get_actions_for((*idea_it)->purpose)->size() > 0) {
                    best_val = val;
                    best_target = (*obj_it);
                    best_purpose = (*idea_it)->purpose;
                }
            }
        }
    }
    
    ideas = (*decider->my_mind->module->ideas)[ENT_TYPE_FEATURE];    
    if(ideas != NULL) {
        set<feature*>::iterator feat_it = features->begin();
        for(; feat_it != features->end(); ++feat_it) {
           
            idea_it = ideas->begin();
            for(; idea_it != ideas->end(); ++idea_it) {
                int val = idea_value(decider, (*feat_it), (*idea_it));
                if (val > best_val && decider->get_actions_for((*idea_it)->purpose)->size() > 0) {
                    best_val = val;
                    best_target = (*feat_it);
                    best_purpose = (*idea_it)->purpose;
                }
            }
        }
    }
    
    ideas = (*decider->my_mind->module->ideas)[ENT_TYPE_NONE];
    if(ideas != NULL) {
        idea_it = ideas->begin();
        for(; idea_it != ideas->end(); ++idea_it) {
        
            int val = idea_value(decider, decider, (*idea_it));
            if (val > best_val && decider->get_actions_for((*idea_it)->purpose)->size() > 0) {
                best_val = val;
                best_target = decider;
                best_purpose = (*idea_it)->purpose;
            }
        }
    }
    
    goal * ret = new goal(best_target, best_purpose);
    return ret;
}

/*
    Calclate an attraction value for the idea of doing a certain purpose
    to a certain target.
*/
int AI::idea_value(actor * decider, entity * target, idea * thought) {

    int ret = thought->base;
    vector<void*> * pat = NULL;
    vector<motivator*>::iterator it = thought->motivators->begin();
    
    for(; it != thought->motivators->end();++it) {
    
        if ((*it)->self) {
            (*it)->req->args->add_entity(ARG_ACTION_AGENT, (entity*)decider);
            if (target != NULL) {
                pat = new vector<void*>();
                pat->push_back((void*)target);
                (*it)->req->args->add_vector(ARG_ACTION_PATIENT, pat);
            }            
        } else {
            (*it)->req->args->add_entity(ARG_ACTION_AGENT, (entity*)target);
            if (decider != NULL) {
                pat = new vector<void*>();
                pat->push_back((void*)decider);
                (*it)->req->args->add_vector(ARG_ACTION_PATIENT, pat);
            }
        }
        (*it)->req->args->add_int(ARG_REQUIRE_UNARY_ROLE, ARG_ACTION_AGENT);
        
        if((*it)->req->check() == NULL) {
            ret += (*it)->value;
        }    
    }
    
    return ret;
}

bool AI::take_action(actor * act, goal * g) {
    
    argmap * args = new argmap();
    args->add_actor(ARG_ACTION_AGENT, act);
    vector<void*> * pat = new vector<void*>();
    pat->push_back((void*)g->target);
    args->add_vector(ARG_ACTION_PATIENT, pat);
    
    if (g->purpose < ACTPUR_ABSTRACT && g->purpose != ACTPUR_MOVE) {
    
        // If concrete, find actions, determine range, move if needed
        
        // For each action, see if it's possible or how much work it would take to make
        // it possible, and weigh that against its priority.
        vector<action*> * actions = action::defs_for(act->get_actions_for(g->purpose));
        int best_priority = -1000;
        action * best_action = NULL;
        set<error_t> * errs; // Errors for chosen action
        
        vector<action*>::iterator it = actions->begin();
        for(; it != actions->end(); ++it) {
            
            set<error_t> * cur_errs = act->test_action(*it, args);
            int mod_pri = (*it)->priority - act->effort_heuristic(*it, args, cur_errs);
            
            if ((best_action == NULL || best_priority < mod_pri)
             && (cur_errs == NULL || !cur_errs->count(ERR_FAIL)) ) {
              
                best_action = (*it);
                best_priority = mod_pri;
                errs = cur_errs;
            }
        }
        
        if (best_action != NULL) {
        
            if (errs == NULL) {
                // No errors, use the action right away
                act->execute_action(best_action, args, false);
                return true;
            } else {
            
                // We want to use this action, but need to do something else first
                error_t to_fix = act->easiest_to_fix(best_action, args, errs);
                goal * new_goal = goal_to_solve(act, g, to_fix);

                if (new_goal != NULL) {
                    return take_action(act, new_goal);
                } else {
                    return false;
                }
            }
        
        } else {
            // We didn't have any available actions to accomplish this purpose - we'll have
            // to try something else.
            return false;
        }
        
    } else if(g->purpose == ACTPUR_MOVE) {
        // Movement is a special case due to the need for pathing
        act->move_toward((mapentity *)g->target);
        return true;
    } else {
        // If abstract, resolve that here
        if (g->purpose == ACTPUR_WANDER) {
            tile * destination;
            int x, y;
            
        }
    }
    
    return false;
}

/*
    We want to do the specified goal, but need a new subgoal to solve
    the specified error.
*/
goal * AI::goal_to_solve(actor * act, goal * g, error_t err) {

    
    switch(err) {
        case ERR_BAD_INPUT:
        case ERR_ENTITY_TYPE:
        case ERR_CANT_TARGET_SELF:
        case ERR_MUST_TARGET_SELF:
            return NULL;
        break;
    
        case ERR_TOO_FAR:
        {
            return new goal(g->target, ACTPUR_MOVE);
        }
        break;
        
        case ERR_TOO_CLOSE:
        {
            // Find some way to move away from things
        }
        break;
        
        case ERR_NEED_WEAPON:
        {
            // Equip a weapon
        }
        break;
        
        default:
        break;
    }
    
    return NULL;
}
