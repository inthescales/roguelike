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
#include "pathing.h"
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

vector<action*> * actor::get_actions() {

    vector<action*> * r = mapentity::get_actions();
    
    // Get actions from my inventory
    vector<object*>::iterator item_it = inventory->begin();
    for(; item_it != inventory->end(); ++item_it) {
    
        vector<action*> * acts = (*item_it)->get_actions();
        vector<action*>::iterator inv_it = acts->begin();
        for(; inv_it != acts->end(); ++inv_it) {
        
            if(!(*inv_it)->equipOnly || (*item_it)->equipped) {
                r->insert(r->end(), *inv_it);
            }
        }
    }
    
    return r;
}

vector<action*> * actor::get_actions_for(actionPurpose_t purp) {

    vector<action*> * all = get_actions();

    vector<action*> * r = new vector<action*>();
    for(int i = 0; i < all->size(); ++i) {
        if(all->at(i)->purpose == purp) {
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

vector<void*> * actor::select_target(targetActionBlock * in) {

    if (this == act_player) {
        return UI::prompt_target(in);
    } else {
        // Probably won't need this with current AI routine, but may
        // be useful at some point
    }
    
    return NULL;
}

/*
    Estimate amount of effort (roughly, time) needed to perform an action.
*/
int actor::effort_heuristic(action * ac, argmap * args, vector<error*> * errors) {

    int effort = 0;
    
    if (errors != NULL) {
        vector<error*>::iterator err_it = errors->begin();
        for(;err_it != errors->end(); ++err_it) {
            
            effort += effort_to_fix(ac, args, *err_it);
        }
    }
    
    return effort;
}

error * actor::easiest_to_fix(action * ac, argmap * args, vector<error*> * errors) {

    error * easiest = NULL;
    int effort = 1000;
    
    vector<error*>::iterator it = errors->begin();
    for(; it != errors->end(); ++it) {
        
        int eff = effort_to_fix(ac, args, *it);
        if (easiest == NULL || eff < effort) {
            easiest = *it;
            effort = eff;
        }
    }
    
    return easiest;
}

int actor::effort_to_fix(action * ac, argmap * args, error * err) {

    switch(err->code) {
        
        case ERR_BAD_INPUT:
        case ERR_ENTITY_TYPE:
        case ERR_CANT_TARGET_SELF:
        case ERR_MUST_TARGET_SELF:
            return 1000;
        break;
    
        case ERR_TOO_FAR:
        {
            entity * target = (entity*)args->get_vector(ARG_ACTION_PATIENT)->front();
            int dist = pathing::distance_between(this, (mapentity*)target);
            return std::min(0, dist - ac->args->get_int(ARG_TARGET_MAX_DISTANCE));
        }
        break;
        
        case ERR_TOO_CLOSE:
        {
            entity * target = (entity*)args->get_vector(ARG_ACTION_PATIENT)->front();
            int dist = pathing::distance_between(this, (mapentity*)target);
            return std::min(0, ac->args->get_int(ARG_TARGET_MIN_DISTANCE) - dist);
        }
        break;
        
        // Generic version - TODO - for when I introduce error objects
        /*case ERR_NEED_WEAPON:
        {
            int slot = args->get_int(ARG_AI_REQUIRE_EQUIPPED_SLOT)
        }*/
        
        case ERR_NEED_WEAPON:
        {
            equip_slot slot = ES_MAINHAND;
            if (objects_for_slot(slot)->size() > 0) {
                return 1;
            } else {
                return 1000;
            }
        }
        break;
        
        default:
        break;
    }
    
    return 0;
}

vector<tile*> * actor::path_to(mapentity * target) {

    vector<tile*> * ret;
    
    // Get ok movement types
    vector<action*> * actions = get_actions_for(ACTPUR_MOVE);

    ret = path_astar(target, actions);
    
    return ret;
}

vector<tile*> * actor::path_astar(mapentity * target, vector<action*> * moves) {

    vector<tile*> * ret = new vector<tile*>();
    vector<action*>::iterator mov_it;
    tile * start = &current_map->tiles[x][y];
    tile * goal = &current_map->tiles[target->x][target->y];
   
    vector<tile*> * closed_set = new vector<tile*>();
    vector<tile*> * open_set = new vector<tile*>();
    open_set->push_back(start);
    std::map<tile*,tile*> * came_from = new std::map<tile*,tile*>();
    std::map<tile*, float> * g_score = new std::map<tile*, float>();
    std::map<tile*, float> * f_score = new std::map<tile*, float>();;
    (*g_score)[start] = 0;
    (*f_score)[start] = heuristic_cost_estimate(start, goal);
    
    tile * current;
    float tent_g;
    while(open_set->size() > 0) {
    
        current = get_lowest_f(open_set, f_score);
        if (current == goal) {
            return reconstruct_path(came_from, goal);
        }
        
        open_set->erase(std::find(open_set->begin(), open_set->end(), current));
        closed_set->push_back(current);
        vector<tile*> * adj = pathing::adjacent_to(current);
        for(int i = 0; i < adj->size(); ++i) {
            tile * neighbor = adj->at(i);
            if(std::find(closed_set->begin(), closed_set->end(), neighbor) != closed_set->end()) {
                continue;
            }
            
            tent_g = (*g_score)[current] + pathing::distance_between(current, neighbor);
            if (neighbor->x != current->x && neighbor->y != current->y) {
                tent_g += .001; // Prefer straight paths
            }
            
            bool neigh_in_open = std::find(open_set->begin(), open_set->end(), neighbor) != open_set->end();
            if(!neigh_in_open || tent_g < (*g_score)[neighbor]) {
                
                if (current != start) {
                    (*came_from)[neighbor] = current; // Don't include start tile in path
                }
                (*g_score)[neighbor] = tent_g;
                (*f_score)[neighbor] = (*g_score)[neighbor] + heuristic_cost_estimate(neighbor, goal);
                if (!neigh_in_open) {
                    open_set->push_back(neighbor);
                }
            }
        }
    }
    return NULL;
}

tile * get_lowest_f(vector<tile*> * tiles, std::map<tile*, float> * vals) {

    tile * best_t = NULL;
    float best_v = 0;
    vector<tile*>::iterator it = tiles->begin();
    for(; it != tiles->end(); ++it) {
        if(best_t == NULL || (*vals)[(*it)] < best_v) {
            best_t = *it;
            best_v = (*vals)[(*it)];
        }
    }
    
    return best_t;
}

float heuristic_cost_estimate(tile * start, tile * goal) {

    float r = pathing::distance_between(start, goal);
    return r - 1.0;
}

vector<tile*> * reconstruct_path(std::map<tile*,tile*> * came_from, tile * goal) {

    vector<tile*> * ret = new vector<tile*>();
    ret->push_back(goal);
    tile * current = goal;
    while(came_from->count(current) > 0) {
    
        current = (*came_from)[current];
        ret->push_back(current);
    }
    
    return ret;
}

bool actor::move_toward(mapentity * target) {
    
    vector<action*> * actions = get_actions_for(ACTPUR_MOVE);
    vector<action*>::iterator it = actions->begin();
    tile * cur_tile = &current_map->tiles[x][y];
    
    //vector<tile*> * line = tile::line_between(this, target);
    vector<tile*> * line = path_astar(target, NULL);
    action * best_action = NULL;
    int best_dist = 0;
    
    for(;it != actions->end(); ++it) {
    
        int range = -1;

        for(int i = 0; i < line->size() && (range == -1 || i < range); ++i) {
            
            if (best_action == NULL
             || (can_travel_with(*it, cur_tile, line->at(i))->size() == 0) && best_dist < i ) {
                best_action = (*it);
                best_dist = i;
                continue;
            }
        }        
    }
    
    if (best_action == NULL) {
        return false;
    }
    
    argmap * args = new argmap();
    args->add_actor(ARG_ACTION_AGENT, this);
    vector<void*> * pat = new vector<void*>();
    pat->push_back((void*)line->at(best_dist));
    args->add_vector(ARG_ACTION_LOCATION, pat);
    best_action->execute(args, false);
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

bool actor::can_travel(tile * to) {

    tile * cur = &(current_map->tiles[x][y]);
    return can_travel(cur, to);
}

// Returns true if this actor is able to enter the tile
bool actor::can_travel(tile * from, tile * to) {

    vector<action*> * r = how_to_travel(from, to);
    if (r->size() == 0) {
        return false;
    }
    
    return true;
}

// Returns list of movement actions that can be used to move to the specified tile.
vector<action*> * actor::how_to_travel(tile * from, tile * to) {

    vector<action*> * ret = new vector<action*>();
    vector<action*> * actions = get_actions_for(ACTPUR_MOVE);
    vector<action*>::iterator it = actions->begin();;
    for(;it != actions->end(); ++it) {
        if (can_travel_with(*it, from, to)->size() == 0) {
            ret->push_back(*it);
        }
    }
    
    return ret;
}

// Returns true if the actor could move from one tile to another using the specified action
vector<error*> * actor::can_travel_with(action * ac, tile * from, tile * to) {
    
    argmap * args = new argmap();
    args->add_actor(ARG_ACTION_AGENT, this);
    vector<void*> * pat = new vector<void*>();
    pat->push_back((void*)to);
    args->add_vector(ARG_ACTION_LOCATION, pat);
    action_resp * resp = ac->test(args);
    return resp->errors;
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