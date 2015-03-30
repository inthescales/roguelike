#include "action.h"
#include "actor.h"
#include "actclass.h"
#include "AI.h"
#include "error.h"
#include "feature.h"
#include "interface.h"
#include "map.h"
#include "object.h"
#include "objclass.h"
#include "rogue.h"
#include "stringutils.h"
#include "tile.h"
#include "timer.h"
#include "window.h"

#include <algorithm>

using std::pair;
using std::ostringstream;

// Setup and teardown =================================

actor::actor(short code){

	my_class = (entityclass*)aclass[code];
    individual_name = "";
	my_mind = new mind(((actclass *)my_class)->aimod);
	
	level = 1;
	maxHP = HP = 5;
	gold = 0;
	state = 0; //STATE_STAND;
    
    init();
	
}

void actor::init() {

    mapentity::init();
    
    inventory = new vector<object *>();
    
    for(int i = 0; i < ES_MAX; ++i){
		equipped_item[i] = NULL;
	}
}

// Basic info functions ==========================

actclass * actor::get_class(){
    return (actclass *)entity::get_class();
}

string actor::get_name_color() {
    return color_string(entity::get_name(), get_color());
}

vector<int> * actor::get_actions() {

    vector<int> * r = mapentity::get_actions();
    
    // Get actions from my inventory
    vector<object*>::iterator item_it = inventory->begin();
    for(; item_it != inventory->end(); ++item_it) {
        vector<int> * acts = (*item_it)->get_actions();
        vector<int>::iterator inv_it = acts->begin();
        for(; inv_it != acts->end(); ++inv_it) {
            if(!actiondef[(*inv_it)]->equipOnly || (*item_it)->equipped) {
                r->insert(r->end(), *inv_it);
            }
        }
    }
    
    return r;
}

vector<int> * actor::get_actions_for(actionPurpose_t purp) {

    vector<int> * all = get_actions();

    vector<int> * r = new vector<int>();
    for(int i = 0; i < all->size(); ++i) {
        if(actiondef[all->at(i)]->purpose == purp) {
            r->push_back(all->at(i));
        }
    }
    
    return r;
}

// Stats ================================================

/*
	Returns the current modified stat specified by the input.
	Ideas for later:
		- Store this somewhere to avoid recalculation
		- Add a parameter specifying a code for what types, if any, of modifiers apply
*/
int actor::get_stat(stats_t code){

	// If this is a calculated stat, process that elsewhere
	if (code > CSTAT_MIN )
		return get_calc_stat(code);

	int base, equipment, condition, total;
	
	// Take base stat from actor class
	base = get_class()->get_stat(code);
	
	// Check each equipped item and add any stat modifiers provided
	equipment = get_equip_stat(code);
	
	// Check all conditions and add any stat modifiers
	condition = get_cond_stat(code);
	
	total = base + equipment + condition;
	
	return total;
}

int actor::get_equip_stat(stats_t code) {

	int ret = 0;
	
	for(int i = 0; i < ES_MAX; ++i){
	
		if(equipped_item[i] != 0){
			
			int value = equipped_item[i]->get_stat(code);
			if(value != -1) {
				ret += value;
			}
		}
	}
	
	return ret;
}

int actor::get_calc_stat(stats_t code){

	int val = 0;
	
	if(code == CSTAT_WEP_DMG){
	
		val = get_stat(ASTAT_BODY) + get_equip_stat(OSTAT_WEP_DMG);
	} else if(code == CSTAT_BLOCK){
		
		val = (get_stat(ASTAT_BODY) / 2) + get_equip_stat(OSTAT_ARM_BLOCK);
	}
		
	return val;
}

// Flags ================================================

bool actor::has_flag(flags_t code){

	// Take base stat from actor class
	if (get_class()->has_flag(code)){
        return true;
    }
	
	// Check each equipped item and add any stat modifiers provided
	if (has_equip_flag(code)) {
        return true;
    }
	
	// Check all conditions and add any stat modifiers
	if (has_cond_flag(code)) {
        return true;
    }

    return false;
}

bool actor::has_equip_flag(flags_t code){

	for(int i = 0; i < ES_MAX; ++i){
		if(equipped_item[i] != 0){
			
			if(equipped_item[i]->has_flag(code)) {
				return true;
			}
		}
	}
	
	return false;
}

// EFFECTS ==================================

void actor::resolve_trigger(trigger_t trigger, argmap * args)
{
    argmap * my_map = new argmap(args);
    my_map->add_actor(ARG_HOLDER_ACTOR, this);
    
    mapentity::resolve_trigger(trigger, args);
    
    // Effects from items in inventory
	vector<object*>::iterator oit = inventory->begin();
	for(; oit != inventory->end(); ++oit) {
	
        (*oit)->resolve_trigger(trigger, my_map);
	}

}

// AI ====================================================================

// TODO - make this return set of problems
act_can_t actor::check_can(action * ac, entity * targ) {

    if (ac->args->has_value(ARG_AI_TARG_TYPE)) {
        ai_target_type_t target_type = (ai_target_type_t)ac->args->get_int(ARG_AI_TARG_TYPE);
        
        if (targ == this && target_type != AI_TARG_SELF) {
            return ACTCAN_CANT_USE;
        } else
        if (targ != this && target_type == AI_TARG_SELF) {
            return ACTCAN_CANT_USE;
        }
    }

    if (ac->args->has_value(ARG_AI_MIN_DISTANCE)) {
        int dist = tile::distance_between(this, (mapentity*)targ);
        if (dist < ac->args->get_int(ARG_AI_MIN_DISTANCE)) {
            return ACTCAN_TOO_CLOSE;
        }
    }
    
    if (ac->args->has_value(ARG_AI_MAX_DISTANCE)) {
        int dist = tile::distance_between(this, (mapentity*)targ);
        if (dist > ac->args->get_int(ARG_AI_MAX_DISTANCE)) {
            return ACTCAN_TOO_FAR;
        }
    }
    
    if (ac->args->has_value(ARG_AI_REQUIRE_EQUIPPED)) {
        int slot = ac->args->get_int(ARG_AI_REQUIRE_EQUIPPED);
        if (slot >= ES_MAX || equipped_item[slot] == NULL) {
            return ACTCAN_NEED_EQUIP;
        }
    }
    
    return ACTCAN_CAN_USE;
}

// TODO - make this take in a set of reasons and process them
int actor::check_effort(action * ac, entity * targ) {

    if (ac->args->has_value(ARG_AI_TARG_TYPE)) {
        ai_target_type_t target_type = (ai_target_type_t)ac->args->get_int(ARG_AI_TARG_TYPE);
        
        if (targ == this && target_type != AI_TARG_SELF) {
            return -1000;
        } else
        if (targ != this && target_type == AI_TARG_SELF) {
            return -1000;
        }
    }

    if (ac->args->has_value(ARG_AI_MIN_DISTANCE)) {
        int dist = tile::distance_between(this, (mapentity*)targ);
        int diff = std::min(0, ac->args->get_int(ARG_AI_MIN_DISTANCE) - dist);
        if (diff > 0) {
            return diff;
        }
    }
    
    if (ac->args->has_value(ARG_AI_MAX_DISTANCE)) {
        int dist = tile::distance_between(this, (mapentity*)targ);
        int diff = std::min(0, dist - ac->args->get_int(ARG_AI_MAX_DISTANCE));
        if (diff > 0) {
            return diff;
        }
    }
    
    if (ac->args->has_value(ARG_AI_REQUIRE_EQUIPPED)) {
        equip_slot slot = (equip_slot)ac->args->get_int(ARG_AI_REQUIRE_EQUIPPED);
        if (slot < ES_MAX && equipped_item[slot] == NULL) {
            if (objects_for_slot(slot)->size() > 0) {
                return /*1*/0;
            } else {
                return -1000;
            }
        }
    }
    
    return 0;

}

bool actor::move_toward(mapentity * target) {
    
    vector<action*> * actions = action::defs_for(get_actions_for(ACTPUR_MOVE));
    vector<action*>::iterator it = actions->begin();
    
    vector<tile*> * line = tile::line_between(this, target);
    action * best_action = NULL;
    int best_dist = 0;
    
    for(;it != actions->end(); ++it) {
    
        int range = -1;
        if ((*it)->args->has_value(ARG_AI_MAX_DISTANCE)) {
            range = (*it)->args->get_int(ARG_AI_MAX_DISTANCE);
        }
        for(int i = 0; i < line->size() && (range == -1 || i < range); ++i) {
            
            if (best_action == NULL || best_dist < i
             || can_move_to(*it, line->at(i))) {
                best_action = (*it);
                best_dist = i;
                break;
            }
        }        
    }
    
    if (best_action == NULL) {
        return false;
    }
    
    argmap * args = new argmap();
    vector<void*> * pat = new vector<void*>();
    pat->push_back((void*)line->at(best_dist));
    args->add_vector(ARG_ACTION_PATIENT, pat);
    execute_action(best_action, args, false);
    return true;
}

bool actor::can_move_to(action * ac, tile * t) {

    int dist = tile::distance_between(this, t);
    act_can_t can = check_can(ac, t);
    
    if (can != ACTCAN_CAN_USE) {
        return false;
    }

    if ((ac->args->has_flag(FLAG_ACTION_WALK) && !t->has_flag(FLAG_TILE_CAN_WALK))
     || (ac->args->has_flag(FLAG_ACTION_SWIM) && !t->has_flag(FLAG_TILE_CAN_SWIM))
     || (ac->args->has_flag(FLAG_ACTION_FLY) && !t->has_flag(FLAG_TILE_CAN_FLY))) {
     
        return false;
    }
    
    return true;
}

vector<object*> * actor::objects_of_type(object_type t, object_subtype st) {

    vector<object*> * ret = new vector<object*>();

    vector<object*>::iterator it = inventory->begin();
    for(;it != inventory->end(); ++it) {
    
        if((t == 0 || (*it)->get_type() == t)
        || (st == 0 || (*it)->get_subtype() == st)) {
            ret->push_back(*it);
        }        
    }

    return ret;
}

vector<object*> * actor::objects_for_slot(equip_slot slot) {

    vector<object*> * ret = new vector<object*>();

    vector<object*>::iterator it = inventory->begin();
    for(;it != inventory->end(); ++it) {
    
        if(pick_slot_for(*it) == slot) {
            ret->push_back(*it);
        }        
    }

    return ret;
}

// Action and turn setup ====================================================================

// get_actions

int actor::take_turn() {

    if (this == act_player) {
        UI::get_action(); // This should return delay until next turn
    } else {
        goal * my_goal = AI::select_goal(this);
        AI::take_action(this, my_goal);
    }
    
    return 5; // Return time until next action - effect resolutio will handle requeueing
}

void actor::queue_turn(int t) {
    argmap * args = new argmap();
    args->add_actor(ARG_ACTION_AGENT, this);
    map_current->add_timer(new timer(new effect(EFF_TURN), args, t, 0, 0));
}

bool actor::execute_action(action * in) {
    
    return execute_action(in, NULL, true);
}

// Execute an action, including selecting targets. Returns whether or not the action
// completed. If false, actor turn should not be spent.
bool actor::execute_action(action * in, argmap * args, bool get_targets) {

    if (in->blocks == NULL) {
        return false;
    }

    actionBlock * curBlock;
    
    bool failedReq = false;
    // implement stack for loops
    argmap * roles = new argmap(); // NOT using normal arg keys. Using action role keys instead.
    roles->add_actor((args_t)ACTROLE_AGENT, this);
    vector<void*> * nvect;
    
    if (args) {
        if (args->has_value(ARG_ACTION_AGENT)) roles->add_actor((args_t)ACTROLE_AGENT, args->get_actor(ARG_ACTION_AGENT));
        if (args->has_value(ARG_ACTION_PATIENT)) roles->add_vector((args_t)ACTROLE_PATIENT, args->get_vector(ARG_ACTION_PATIENT));
        if (args->has_value(ARG_ACTION_INSTRUMENT)) roles->add_vector((args_t)ACTROLE_INSTRUMENT, args->get_vector(ARG_ACTION_INSTRUMENT));
    }

    // Process each block in the action in order
    for (int i = 0; i < in->blocks->size(); ++i) {
    
        curBlock = in->blocks->at(i);
        
        // If we failed the last requirement block and haven't seen an end yet, skip instruction.
        if (failedReq && !(curBlock->block_type == REQUIREMENT_BLOCK && ((requirementActionBlock *)curBlock)->endBlock)) continue;
        
        if (curBlock->block_type != TARGET_BLOCK) {
            if(roles->has_value((args_t)ACTROLE_AGENT)) {
                curBlock->args->add_actor(ARG_ACTION_AGENT, roles->get_actor((args_t)ACTROLE_AGENT));
            }
            if(roles->has_value((args_t)ACTROLE_PATIENT)) {
                curBlock->args->add_vector(ARG_ACTION_PATIENT, roles->get_vector((args_t)ACTROLE_PATIENT));
            }
            if(roles->has_value((args_t)ACTROLE_INSTRUMENT)) {
                curBlock->args->add_vector(ARG_ACTION_INSTRUMENT, roles->get_vector((args_t)ACTROLE_INSTRUMENT));
            }
        }
        
        switch (curBlock->block_type) {
        
            // TARGET BLOCK - get vector returned by target function, set semantic role
            case TARGET_BLOCK:
                if (get_targets) {
                    nvect = select_target((targetActionBlock *)curBlock);
                    switch(((targetActionBlock *)curBlock)->position) {
                    
                        case ACTROLE_AGENT:
                            roles->add_actor((args_t)ACTROLE_AGENT, (actor*)nvect->at(0));
                        break;
                        case ACTROLE_PATIENT:
                        case ACTROLE_PATIENT_2:
                            roles->add_vector((args_t)ACTROLE_PATIENT, nvect);
                        break;
                        case ACTROLE_INSTRUMENT:
                            roles->add_vector((args_t)ACTROLE_INSTRUMENT, nvect);
                        break;
                        case ACTROLE_LOCATION:
                            roles->add_vector((args_t)ACTROLE_LOCATION, nvect);
                        break;
                    }
                }
            break;
            
            // EXTRACT BLOCK - take role, run extraction function, store in new role
            case EXTRACT_BLOCK:
            
                switch (((extractActionBlock *)curBlock)->from_position) {
                    case ACTROLE_AGENT:
                        nvect = new vector<void*>();
                        nvect->push_back((void*)roles->get_actor((args_t)ACTROLE_AGENT));
                    break;
                    default:
                        nvect = roles->get_vector((args_t)((extractActionBlock *)curBlock)->from_position);
                    break;
                }
            
                switch (((extractActionBlock *)curBlock)->extract_type) {
                    case EXT_ACTOR:
                        nvect = (vector<void*>*)UI::extract_actors((vector<tile*> *)nvect, curBlock->requirements);
                    break;
                    case EXT_OBJECTS:
                        nvect = (vector<void*>*)UI::extract_objects((vector<tile*> *)nvect, curBlock->requirements);
                    break;
                    case EXT_INVENTORY:
                        nvect = (vector<void*>*)UI::extract_inventories((vector<tile*> *)nvect, curBlock->requirements);
                    break;
                    case EXT_FEATURE:
                        nvect = (vector<void*>*)UI::extract_features((vector<tile*> *)nvect, curBlock->requirements);
                    break;
                }
                
                switch(((extractActionBlock *)curBlock)->to_position) {
                
                    case ACTROLE_AGENT:
                        roles->add_actor((args_t)ARG_ACTION_AGENT, (actor*)nvect->at(0));
                    break;
                    default:
                        roles->add_vector((args_t)((extractActionBlock *)curBlock)->to_position, nvect);
                    break;
                }
            break;
            
            // EFFECT BLOCK - set roles, process effect
            case EFFECT_BLOCK:
                do_effect(curBlock->args, ((effectActionBlock *)curBlock)->eff);
            break;
                
            // REQUIREMENT BLOCK - set roles, check for requirements
            case REQUIREMENT_BLOCK:
                
                // TODO - Implement loops
                failedReq = !((requirementActionBlock *)curBlock)->evaluate();
                if (failedReq && ((requirementActionBlock *)curBlock)->critical) {
                    // Failed requirement means action cannot count as completed
                    return false;
                }
            break;
          
        }
        
    }
    
    return true;
}

vector<void*> * actor::select_target(targetActionBlock * in) {

    if (this == act_player) {
        return UI::prompt_target(in);
    } else {
        //return AI::select_target(in);
    }
    
    return NULL;
}

// Specific actions =============================

void actor::move(tile * new_tile) {

	tile * old_tile = &map_current->tiles[x][y];
    old_tile->my_actor = NULL;
    new_tile->my_actor = this;
    x = new_tile->x;
    y = new_tile->y;
    
    // Move the window's frame if necessary
    if (this == act_player) {
        if (x < win_world->frame_x + scroll_border && win_world->frame_x > 0) {
            win_world->frame_x = x - scroll_border;
            win_world->should_update = true;
        } else if ( x > win_world->frame_x + win_world->width-1 - scroll_border && win_world->frame_x < current_map->width - win_world->width) {
            win_world->frame_x = x + scroll_border - (win_world->width-1);
            win_world->should_update = true;
        }
        
        if (y < win_world->frame_y + scroll_border && win_world->frame_y > 0) {
            win_world->frame_y = y - scroll_border;
            win_world->should_update = true;
        } else if ( y > win_world->frame_y + win_world->height-1 - scroll_border && win_world->frame_y < current_map->height - win_world->height) {
            win_world->frame_y = y + scroll_border - (win_world->height-1);
            win_world->should_update = true;
        }
    }
}

void actor::walk(tile * new_tile) {

	tile * old_tile = &map_current->tiles[x][y];
    argmap * args = new argmap();
    args->add_actor(ARG_ACTION_AGENT, this);
    old_tile->resolve_trigger(TRG_TILE_WALK_OUT, args);
    move(new_tile);
    new_tile->resolve_trigger(TRG_TILE_WALK_IN, args);
    resolve_trigger(TRG_ACT_WALK, new argmap());
    enter_tile(new_tile);
}

void actor::swim(tile * new_tile) {

	tile * old_tile = &map_current->tiles[x][y];
    argmap * args = new argmap();
    args->add_actor(ARG_ACTION_AGENT, this);
    
	old_tile->my_actor = NULL;
    old_tile->resolve_trigger(TRG_TILE_SWIM_OUT, args);
	new_tile->my_actor = this;
    new_tile->resolve_trigger(TRG_TILE_SWIM_IN, args);
    resolve_trigger(TRG_ACT_SWIM, new argmap());
    enter_tile(new_tile);
}

void actor::fly(tile * new_tile) {

	tile * old_tile = &map_current->tiles[x][y];
    argmap * args = new argmap();
    args->add_actor(ARG_ACTION_AGENT, this);
    
	old_tile->my_actor = NULL;
    old_tile->resolve_trigger(TRG_TILE_FLY_OUT, args);
	new_tile->my_actor = this;
    new_tile->resolve_trigger(TRG_TILE_FLY_IN, args);
    resolve_trigger(TRG_ACT_FLY, new argmap());
    enter_tile(new_tile);
}

void actor::enter_tile(tile * t) {
    
	// If this tile has objects, print a message
    if(t->my_objects->size() == 1){
    
        object * the_obj = t->my_objects->back();
        if(this == act_player) {
            win_output->print("You see a " + the_obj->get_name_color() + " here.");
        }
    } else if(t->my_objects->size() > 1) {
        win_output->print("There are " + int_string(t->my_objects->size()) + " objects here.");
    }
}

// ITEM INTERACTION ==================================================
// These functions assume that the action can be performed by this actor
// on the specified object. This should be confirmed beforehand.

// Take an object from the ground
bool actor::pick_up(object * target, tile * place){
	
	get_object(target);
	place->remove_object(target);
	
	return true;
}

// Take an object. Derive object's position
bool actor::pick_up(object * target) {

    // TODO - Check whether the thing's in an inventory. Use a flag maybe?
    tile * current = &(target->current_map->tiles[target->x][target->y]);
    return pick_up(target, current);
}

bool actor::drop(object * item){

    if (item->equipped) {
        win_output->print("That item is equipped.");
        bool uneq = UI::prompt_yesno("Unequip it?");
        if (uneq) {
            int err = can_unequip(item);
            if (err == ERR_NONE) {
                unequip(item);
            } else {
                win_output->print(error_string[err]);
            }
        } else {
            win_output->print(error_string[ERR_CANCELLED]);
            return false;
        }
    }

    tile * current = &(current_map->tiles[x][y]);
    remove_object(item);
    current->get_object(item);
}

int actor::pick_slot_for(object * item) {

    int type = item->get_class()->type;
    int subtype = item->get_class()->subtype;
    
    if(type == OT_WEAPON){
		return ES_MAINHAND;
	} else if(type == OT_ARMOR){
		switch(subtype){
			case OST_HAT:
			case OST_HELM:
				return ES_HEAD;
			case OST_SUIT:
				return ES_BODY;
			case OST_CLOAK:
				return ES_BACK;
			case OST_GLOVES:
				return ES_HANDS;
			case OST_BOOTS:
				return ES_FEET;
			default:
				return -1;
		}
	} else if(type == OT_ACCESSORY){
		switch(subtype){
			case OST_RING:
				return ES_RING1;
			case OST_AMULET:
				return ES_AMULET;
			default:
				return -1;
		}
	} else if(type == OT_TOOL){
		switch(subtype){
			case OST_LAMP:
				return ES_LIGHT;
			default:
				return -1;
		}
	} else {
		return -1;
    }
}

bool actor::equip(object * item) {

    equip(item, pick_slot_for(item));
}

// Equip an object
bool actor::equip(object * item, int slot){

    if(slot == ES_RING1 && equipped_item[ES_RING1] != NULL){
        slot = ES_RING2;
    }
    
    object * old_item = act_player->equipped_item[slot];
    if(this == act_player && old_item != NULL){
        bool temp = UI::prompt_yesno("Replace " + old_item->get_name() + "?");
        if(temp){
            int err = can_unequip(old_item);
            if (err != ERR_NONE) {
                act_player->unequip(old_item);
            } else {
                win_output->print(error_string[err]); // Consider moving this to error.cpp
            }
        } else {
            return ERR_CANCELLED;
        }
    }
    
	do_equip(item, slot);
    
    if (this == act_player) {
        win_output->print("You equip the " + item->get_name_color());
    } else {
        win_output->print("The " + get_name_color() + " equips the " + item->get_name_color());
    }
}

// Unequip an object
bool actor::unequip(object * item){

	for(int i = 0; i < ES_MAX; ++i){
		
		if(equipped_item[i] == item){
		
            do_unequip(item, i);
            
            if (this == act_player) {
                win_output->print("You unequip the " + item->get_name_color());
            } else {
                win_output->print("The " + get_name_color() + " unequips the " + item->get_name_color());
            }
            
			return true;
		}
	}
	
	return false;
}

// Eat an object
bool actor::eat(object * item){
	
    argmap * m = new argmap();
	m->add_actor(ARG_ACTION_AGENT, this);
    if(this == act_player) {
        win_output->print("You eat the " + item->get_name_color() +".");
	}
    item->resolve_trigger(TRG_EAT, m); 
}

// Drink an object
bool actor::drink(object * item){
	
    argmap * m = new argmap();
	m->add_actor(ARG_ACTION_AGENT, this);
    if(this == act_player) {
        win_output->print("You drink the " + item->get_name_color() +".");
	}
    item->resolve_trigger(TRG_DRINK, m); 
}

// Read an object
bool actor::read(object * item){
	
	
}

// Use an object
bool actor::use(object * item){
	
	
}

// Open a feature, like a door
bool actor::open_feature(feature * feat){
	
    if(this == act_player) {
        win_output->print("You open the " + feat->get_name_color() +".");
	}
	feat->change_state(STATE_FEAT_OPEN);
}

// Close a feature, like a door
bool actor::close_feature(feature * feat){
	
    if(this == act_player) {
        win_output->print("You close the " + feat->get_name_color() +".");
	}
	feat->change_state(STATE_FEAT_CLOSED);
}

// Attack a target using equipped weapon
bool actor::strike(actor * target) {
	int damage = get_stat(CSTAT_WEP_DMG) - target->get_stat(CSTAT_BLOCK);
    string name, verb, targetName;
    if (act_player == this) {
        name = "You";
        verb = " attack ";
        if (target == this) {
            targetName = "yourself";
        } else {
            targetName = "the " + target->get_name_color();
        }
    } else {
        name = get_name_color();
        verb = " attacks ";
        if (target == this) {
            targetName = "you";
        } else {
            targetName = "the " + target->get_name_color();
        }
    }
    
	string out = name + verb + targetName + " for " + int_string(damage) + " damage.";
	win_output->print(out);
}

// Attack a target using fists
bool actor::punch(actor * target) {
	int damage = get_stat(ASTAT_BODY) - target->get_stat(CSTAT_BLOCK);
    string name, verb, targetName;
    if (act_player == this) {
        name = "You";
        verb = " punch ";
        if (target == this) {
            targetName = "yourself";
        } else {
            targetName = "the " + target->get_name_color();
        }
    } else {
        name = get_name_color();
        verb = " punches ";
        if (target == this) {
            targetName = "you";
        } else {
            targetName = "the " + target->get_name_color();
        }
    }
    
	string out = name + verb + targetName + " for " + int_string(damage) + " damage.";
	win_output->print(out);
}

// Senses ==================================================

// Helpers ============================================

// Put an item into the actor's inventory, organized by type
void actor::get_object(object * item){
	
	vector<object*>::iterator it = inventory->begin();
	
	for(; it != inventory->end() && (*it)->get_class()->type < item->get_class()->type; ++it);
	
	if(this == act_player){
		char c = UI::get_next_letter();
	
		item->letter = c;
		obj_letter[UI::letter_to_int(c)] = item;
	}
	
	inventory->insert(it, item);
}

// Remove an object from the actor's inventory
bool actor::remove_object(object * item){
	
	obj_letter[UI::letter_to_int(item->letter)] = NULL;
	item->letter = 0;
	vector<object*>::iterator it = std::find( inventory->begin(), inventory->end(), item);
	inventory->erase(it);
	
	return true;
}

// Equip object and manage state
void actor::do_equip(object * item, int slot) {

    equipped_item[slot] = item;
	item->equipped = true;
}

// Unequip object and manage state
void actor::do_unequip(object * item, int slot) {
	
    equipped_item[slot] = NULL;
	item->equipped = 0;
}

// REQUIREMENT FUNCTIONS =========================================

// Returns true if this actor is able to enter the tile
int actor::can_travel(tile * t) {

    if (can_walk(t) || can_swim(t) || can_fly(t)) {
        return ERR_NONE;
    } else {
        return ERR_SILENT;
    }
}

int actor::can_walk(tile * t) {

    feature * feat = t->my_feature;
    if (t->my_actor == NULL && has_flag(FLAG_ACT_CAN_WALK) && t->has_flag(FLAG_TILE_CAN_WALK) && !(feat && feat->has_flag(FLAG_FEAT_NO_WALK))){
        return ERR_NONE;
    }
    
    return ERR_SILENT;
}

int actor::can_swim(tile * t) {

    feature * feat = t->my_feature;
    if (t->my_actor == NULL && has_flag(FLAG_ACT_CAN_SWIM) && t->has_flag(FLAG_TILE_CAN_SWIM) && !(feat && feat->has_flag(FLAG_FEAT_NO_SWIM))){
        return ERR_NONE;
    }
    
    return ERR_SILENT;
}

int actor::can_fly(tile * t) {

    feature * feat = t->my_feature;
    if (t->my_actor == NULL && has_flag(FLAG_ACT_CAN_FLY) && t->has_flag(FLAG_TILE_CAN_FLY) && !(feat && feat->has_flag(FLAG_FEAT_NO_FLY))){
        return ERR_NONE;
    }
    return ERR_SILENT;
}

int actor::can_take(object * obj) {
    
    bool ok = false;
	vector<object*>::iterator it = inventory->begin();
	
	for(; it != inventory->end(); ++it) {
	
        if (object::can_stack(*it, obj)) {
            ok = true;
        }
	}

    if(!ok && inventory->size() < INV_MAX) {
		ok = true;
	}
    
    if (!ok) {
        if (this == act_player) {
            return ERR_CANT_CARRY;
        } else {
            return ERR_SILENT;
        }
    } else {
        return ERR_NONE;
    }
}

int actor::can_drop(object * obj) {

    return ERR_NONE;
}

int actor::can_equip(object * obj) {
    
    int slot = pick_slot_for(obj);

    if (this == act_player) {
        if (slot == -1) {
            return ERR_CANT_EQUIP;
        } else if (obj->equipped) {
            return ERR_ALREADY_EQUIPPED;
        }
    } else {
        if (slot == -1 || obj->equipped) {
            return ERR_SILENT;
        }
    }
    
    return ERR_NONE;
}

int actor::can_unequip(object * obj) {
    
    bool ok = obj->equipped;
    if (!ok && this == act_player) {
        return ERR_NOT_EQUIPPED;
    }
    
    return ERR_NONE;
}

int actor::can_eat(object * obj) {

    bool ok = obj->get_class()->type == OT_FOOD;
    
    if (!ok && this == act_player) {
        return ERR_CANT_EAT;
    }
    
    return ERR_NONE;
}

int actor::can_drink(object * obj) {

    bool ok = obj->get_class()->type == OT_DRINK;
    
    if (!ok && this == act_player) {
        return ERR_CANT_DRINK;
    }
    
    return ERR_NONE;
}

int actor::can_open(feature * feat) {

    return feat->can_open();
}

int actor::can_close(feature * feat) {

    return feat->can_close();
}

int actor::can_strike(actor * actor) {

    if (equipped_item[ES_MAINHAND] == NULL) {
        return ERR_SILENT;
    }
    
    return ERR_NONE;
}

int actor::can_punch(actor * actor) {
  
    return ERR_NONE;
}

// Other simple info =======================

bool actor::is_opaque() {
    return has_flag(FLAG_ACT_OPAQUE);
}