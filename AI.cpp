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

consider_resp::consider_resp(actionPurpose_t p, entity * t, action * a, int d) {

    main_purpose = p;
    sub_purpose = p;
    main_target = t;
    sub_target = t;
    ac = a;
    desire = d;
}

consider_resp::consider_resp(actionPurpose_t p, actionPurpose_t p2, entity * t, entity* t2, action * a, int d) {

    main_purpose = p;
    sub_purpose = p2;
    main_target = t;
    sub_target = t2;
    ac = a;
    desire = d;
}

best_act_resp::best_act_resp(actionPurpose_t p, action * a, entity * t, int e) {

    purpose = p;
    ac = a;
    target = t;
    effort = e;
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

consider_resp * AI::consider_turn(actor * decider) {

    // Find entities for consideration ------
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
    
    // Test ideas for each one --------------
    
    vector<idea*> * ideas;
    vector<idea*>::iterator idea_it;
    consider_resp * best_consider = NULL;
    
    ideas = (*decider->my_mind->module->ideas)[ENT_TYPE_ACTOR];
    if (ideas != NULL) {
        set<actor*>::iterator act_it;
        idea_it = ideas->begin();
        for(; idea_it != ideas->end(); ++idea_it) {
            act_it = actors->begin();
            for(; act_it != actors->end(); ++act_it) {

                int val = idea_value(decider, *act_it, *idea_it);
                if ( best_consider == NULL || val > best_consider->desire) {
                
                    best_act_resp * best_ac = choose_action((*idea_it)->purpose, decider, *act_it);
                    if (best_ac && (best_consider == NULL || val - best_ac->effort > best_consider->desire)) {
                    
                        best_consider = new consider_resp((*idea_it)->purpose, best_ac->purpose, *act_it, best_ac->target, best_ac->ac, val - best_ac->effort);
                    }
                }
            }
        }
    }
    
    ideas = (*decider->my_mind->module->ideas)[ENT_TYPE_OBJECT];
    if (ideas != NULL) {
        set<object*>::iterator obj_it;
        idea_it = ideas->begin();
        for(; idea_it != ideas->end(); ++idea_it) {
            obj_it = objects->begin();
            for(; obj_it != objects->end(); ++obj_it) {

                int val = idea_value(decider, *obj_it, *idea_it);
                if ( best_consider == NULL || val > best_consider->desire) {
                
                    best_act_resp * best_ac = choose_action((*idea_it)->purpose, decider, *obj_it);
                    if (best_ac && (best_consider == NULL || val - best_ac->effort > best_consider->desire)) {
                    
                        best_consider = new consider_resp((*idea_it)->purpose, best_ac->purpose, *obj_it, best_ac->target, best_ac->ac, val - best_ac->effort);
                    }
                }
            }
        }
    }
    
    ideas = (*decider->my_mind->module->ideas)[ENT_TYPE_FEATURE];
    if (ideas != NULL) {
        set<feature*>::iterator feat_it;
        idea_it = ideas->begin();
        for(; idea_it != ideas->end(); ++idea_it) {
            feat_it = features->begin();
            for(; feat_it != features->end(); ++feat_it) {

                int val = idea_value(decider, *feat_it, *idea_it);
                if ( best_consider == NULL || val > best_consider->desire) {
                
                    best_act_resp * best_ac = choose_action((*idea_it)->purpose, decider, *feat_it);
                    if (best_ac && (best_consider == NULL || val - best_ac->effort > best_consider->desire)) {
                    
                        best_consider = new consider_resp((*idea_it)->purpose, best_ac->purpose, *feat_it, best_ac->target, best_ac->ac, val - best_ac->effort);
                    }
                }
            }
        }
    }
    
    ideas = (*decider->my_mind->module->ideas)[ENT_TYPE_TILE];
    if (ideas != NULL) {
        set<tile*>::iterator tile_it;
        idea_it = ideas->begin();
        for(; idea_it != ideas->end(); ++idea_it) {
            tile_it = tiles->begin();
            for(; tile_it != tiles->end(); ++tile_it) {

                int val = idea_value(decider, *tile_it, *idea_it);
                if ( best_consider == NULL || val > best_consider->desire) {
                
                    best_act_resp * best_ac = choose_action((*idea_it)->purpose, decider, *tile_it);
                    if (best_ac && (best_consider == NULL || val - best_ac->effort > best_consider->desire)) {
                    
                        best_consider = new consider_resp((*idea_it)->purpose, best_ac->purpose, *tile_it, best_ac->target, best_ac->ac, val - best_ac->effort);
                    }
                }
            }
        }
    }

    ideas = (*decider->my_mind->module->ideas)[ENT_TYPE_NONE];
    idea_it = ideas->begin();
    if (ideas != NULL) {
        for(; idea_it != ideas->end(); ++idea_it) {

            int val = idea_value(decider, NULL, *idea_it);
            if ( best_consider == NULL || val > best_consider->desire) {
            
                best_act_resp * best_ac = choose_action((*idea_it)->purpose, decider, NULL);
                if (best_ac && (best_consider == NULL || val - best_ac->effort > best_consider->desire)) {
                
                    best_consider = new consider_resp((*idea_it)->purpose, best_ac->purpose, NULL, best_ac->target, best_ac->ac, val - best_ac->effort);
                }
            }
        }
    }
    
    return best_consider;
}

/*
    Calclate an attraction value for the idea of doing a certain purpose
    to a certain target.
*/
int AI::idea_value(actor * decider, entity * target, idea * thought) {

    int ret = thought->base;
    vector<void*> * pat = NULL;
    vector<motivator*>::iterator it = thought->motivators->begin();
    argmap * instanceArgs = new argmap();
    
    for(; it != thought->motivators->end();++it) {
    
        if ((*it)->self) {
            instanceArgs->add_into_vector(ARG_ACTION_AGENT, (entity*)decider);
            if (target != NULL) {
                instanceArgs->add_into_vector(ARG_ACTION_PATIENT, (entity*)target);
            }            
        } else {
            instanceArgs->add_into_vector(ARG_ACTION_AGENT, (entity*)target);
            if (decider != NULL) {
                instanceArgs->add_into_vector(ARG_ACTION_PATIENT, (entity*)decider);
            }
        }
        
        if((*it)->req->evaluate(instanceArgs)->result == REQRES_SUCCESS) {
            ret += (*it)->value;
        }    
    }
    
    return ret;
}

/*
    Choose the best action for accomplishing a purpose and a target, and note how
    much effort it would take to perform that outside the core action itself.
*/
best_act_resp * AI::choose_action(actionPurpose_t purpose, actor * act, entity * target) {

    argmap * args = new argmap();
    args->add_into_vector(ARG_ACTION_AGENT, act);
    args->add_into_vector(ARG_ACTION_PATIENT, target);
    
    if (purpose < ACTPUR_ABSTRACT && purpose != ACTPUR_MOVE) {
    
        // For each action, see if it's possible or how much work it would take to make
        // it possible, and weigh that against its priority.
        vector<action*> * actions = act->get_actions_for(purpose);
        int best_priority = -1000;
        action * best_action = NULL;
        vector<error*> * errs; // Errors for chosen action
        
        vector<action*>::iterator it = actions->begin();
        for(; it != actions->end(); ++it) {
            
            action_resp * test_resp = (*it)->test(args);
            int mod_pri = (*it)->priority - act->effort_heuristic(*it, args, test_resp->errors);
            
            if ((best_action == NULL || best_priority < mod_pri)
             /*&& Later, we want a way to test generally for doability.
                  For now we'll just use a high effort if we can't*/ ) {
              
                best_action = (*it);
                best_priority = mod_pri;
                errs = test_resp->errors;
            }
        }
        
        // Do something with the action we like
        if (best_action != NULL) {
        
            if (errs->size() == 0) {
                // No errors, this is the one we want!
                
                best_act_resp * ret = new best_act_resp(purpose, best_action, target, best_priority);
                return ret;
            } else {
            
                // We want to use this action, but need to do something else first
                error * to_fix = act->easiest_to_fix(best_action, args, errs);
                goal * new_goal = goal_to_solve(act, new goal(target, purpose), to_fix);

                if (to_fix != NULL && new_goal != NULL) {
                    return choose_action(new_goal->purpose, act, new_goal->target);
                } else {
                    return NULL;
                }
            }
        
        } else {
            // We didn't have any available actions to accomplish this purpose - we'll have
            // to try something else.
            return NULL;
        }
        
    } else if(purpose == ACTPUR_MOVE) {
        // Movement is a special case due to the need for pathing
        
        best_act_resp * best = act->best_move_toward((mapentity *)target);
        best_act_resp * ret = new best_act_resp(purpose, best->ac, best->target, 1);

        return ret;
    } else {
        // If abstract, resolve that here
        
        
        /*if (purpose == ACTPUR_WANDER) {
            tile * destination;
            int x, y;
            
        }*/
    }
    
    return NULL;

}

/*
    We want to do the specified goal, but need a new subgoal to solve
    the specified error.
*/
goal * AI::goal_to_solve(actor * act, goal * g, error * err) {

    
    switch(err->code) {
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

int AI::take_action(actor * agent, consider_resp * decision) {

    argmap * args = new argmap();
    args->add_into_vector(ARG_ACTION_AGENT, agent);
    
    args_t a_role = (args_t)decision->ac->args->get_int(ARG_ACTION_TARGET_ROLE);
    
    if (a_role != 0) {
        args->add_into_vector(a_role, decision->sub_target);
    }
    action_resp * actres = decision->ac->execute(args, false);
    return 5;

}
