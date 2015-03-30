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

int AI::idea_value(actor * decider, entity * target, idea * thought) {

    int ret = thought->base;
    
    vector<motivator*>::iterator it = thought->motivators->begin();
    for(; it != thought->motivators->end();++it) {
    
        if(((*it)->self && (*it)->req->check_for(decider))
       || !((*it)->self && (*it)->req->check_for((mapentity*)target))){
            ret += (*it)->value;
        }    
    }
    
    return ret;
}

void AI::take_action(actor * act, goal * g) {
    
    if (g->purpose < ACTPUR_ABSTRACT && g->purpose != ACTPUR_MOVE) {
        // If concrete, find actions, determine range, move if needed
        vector<action*> * actions = action::defs_for(act->get_actions_for(g->purpose));
        int best_priority = -1000;
        action * best_action = NULL;
        
        vector<action*>::iterator it = actions->begin();
        for(; it != actions->end(); ++it) {
            
            int mod_pri = (*it)->priority + act->check_effort(*it, g->target);
            
            if ((best_action == NULL || best_priority < mod_pri)
              /*&& act->check_can(*it, g->target) != ACTCAN_CANT_USE*/) {
              
                best_action = (*it);
                best_priority = mod_pri;
            }
        }
        
        // We've chosen our action, but it might require a subtask
        act_can_t reqstat = act->check_can(best_action, g->target);
        
        if (reqstat == ACTCAN_CAN_USE) {
            // Use the action
            argmap * args = new argmap();
            vector<void*> * pat = new vector<void*>();
            pat->push_back((void*)g->target);
            args->add_vector(ARG_ACTION_PATIENT, pat);
            act->execute_action(best_action, args, false);
        } else {
        
            actionPurpose_t sub_purpose = ACTPUR_NONE;
            entity * new_target = NULL;
        
            switch (reqstat) {
            
                case ACTCAN_TOO_FAR:
                    sub_purpose = ACTPUR_MOVE;
                    new_target = g->target;
                break;
                
                case ACTCAN_NEED_EQUIP:
                    
                break;
                
                default:
                break;
            }
            
            if (sub_purpose != ACTPUR_NONE) {
                take_action(act, new goal(new_target, sub_purpose));
            }
        }
        
    } else if(g->purpose == ACTPUR_MOVE) {
        // Movement is a special case due to the need for pathing
        act->move_toward((mapentity *)g->target);
    } else {
        // If abstract, resolve that here
        if (g->purpose == ACTPUR_WANDER) {
            tile * destination;
            int x, y;
            
        }
    }
}
