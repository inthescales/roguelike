#include "action.h"
#include "actor.h"
#include "actclass.h"
#include "feature.h"
#include "flagset.h"
#include "globals.h"
#include "interface.h"
#include "map.h"
#include "object.h"
#include "objclass.h"
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
	//new->aitype = aclass[code].ai;
	
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
        r->insert(r->end(), acts->begin(), acts->end());
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

// Action and turn setup ====================================================================

// get_actions

int actor::take_turn() {

    if (this == act_player) {
        UI::get_action(); // This should return delay until next turn
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
    actor * cur_agent = this;
    vector<void*> * cur_patient = NULL;
    vector<void*> * cur_instrument = NULL;
    vector<void*> * nvect;
    
    if (args) {
        cur_agent = args->get_actor(ARG_ACTION_AGENT);
        cur_patient = args->get_vector(ARG_ACTION_PATIENT);
        cur_instrument = args->get_vector(ARG_ACTION_INSTRUMENT);
        tile * test = (tile *)args->get_vector(ARG_ACTION_PATIENT)->front();
        int x = 4;
    }
    
    actor * temp;
    // Process each block in the action in order
    for (int i = 0; i < in->blocks->size(); ++i) {
    
        curBlock = in->blocks->at(i);
        
        // If we failed the last requirement block and haven't seen an end yet, skip instruction.
        if (failedReq && !(curBlock->block_type == REQUIREMENT_BLOCK && ((requirementActionBlock *)curBlock)->endBlock)) continue;
        
        if (curBlock->block_type != TARGET_BLOCK) {
            if (cur_agent != NULL) curBlock->args->add_actor(ARG_ACTION_AGENT, cur_agent);
            if (cur_patient != NULL) curBlock->args->add_vector(ARG_ACTION_PATIENT, cur_patient);
            if (cur_instrument != NULL) curBlock->args->add_vector(ARG_ACTION_INSTRUMENT, cur_instrument);
        }
        
        switch (curBlock->block_type) {
        
            // TARGET BLOCK - get vector returned by target function, set semantic role
            case TARGET_BLOCK:
                if (get_targets) {
                    nvect = select_target((targetActionBlock *)curBlock);
                    switch(((targetActionBlock *)curBlock)->position) {
                    
                        case ACTROLE_PATIENT:
                            cur_patient = nvect;
                        break;
                        
                        case ACTROLE_INSTRUMENT:
                            cur_instrument = nvect;
                        break;
                            
                        default:
                        break;
                    }
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

void actor::walk(tile * new_tile) {

	tile * old_tile = &map_current->tiles[x][y];
    argmap * args = new argmap();
    args->add_actor(ARG_ACTION_AGENT, this);
    
	old_tile->my_actor = NULL;
    old_tile->resolve_trigger(TRG_TILE_WALK_OUT, args);
	new_tile->my_actor = this;
    new_tile->resolve_trigger(TRG_TILE_WALK_OUT, args);
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
    new_tile->resolve_trigger(TRG_TILE_SWIM_OUT, args);
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
    new_tile->resolve_trigger(TRG_TILE_FLY_OUT, args);
    resolve_trigger(TRG_ACT_FLY, new argmap());
    enter_tile(new_tile);
}

void actor::enter_tile(tile * t) {

    x = t->x;
    y = t->y;
    
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

// Perform a basic attack at a specified tile
void actor::attack(pair<int,int> offset) {
	tile * place;
	place = &map_current->tiles[x + offset.first][y + offset.second];
	attack(place->my_actor);
}

// Perform a basic attack at a specified actor
void actor::attack(actor * target){
	int damage = get_stat(CSTAT_WEP_DMG) - target->get_stat(CSTAT_BLOCK);
	string out = entity::get_name() + color_string(" attack ", C_RED) + target->entity::get_name() + " for " + int_string(damage) + " damage.";
	win_output->print(out);
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

// Drop an object on the ground
bool actor::drop(object * target, tile * place){
	
	remove_object(target);
	place->get_object(target);
	
	return true;
}

// Equip an object
bool actor::equip(object * item, int slot){

	equipped_item[slot] = item;
	item->equipped = 1;
}

// Unequip an object
bool actor::unequip(object * item){

	for(int i = 0; i < ES_MAX; ++i){
		
		if(equipped_item[i] == item){
		
			equipped_item[i] = NULL;
			item->equipped = 0;
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

// Print a descriptive string. The format depends on whether
// the actor is the player or not.
void actor::print(string a, string b){
	
	if(this == act_player)
		win_output->print(a);
	else
		win_output->print(b);
}

// Returns true if this actor is able to enter the tile
bool actor::can_travel(tile * t) {

    return can_walk(t) || can_swim(t) || can_fly(t);
}

bool actor::can_walk(tile * t) {

    feature * feat = t->my_feature;
    if (t->my_actor == NULL && has_flag(FLAG_ACT_CAN_WALK) && t->has_flag(FLAG_TILE_CAN_WALK) && !(feat && feat->has_flag(FLAG_FEAT_NO_WALK))){
        return true;
    }
    
    return false;
}

bool actor::can_swim(tile * t) {

    feature * feat = t->my_feature;
    if (t->my_actor == NULL && has_flag(FLAG_ACT_CAN_SWIM) && t->has_flag(FLAG_TILE_CAN_SWIM) && !(feat && feat->has_flag(FLAG_FEAT_NO_SWIM))){
        return true;
    }
    
    return false;
}

bool actor::can_fly(tile * t) {

    feature * feat = t->my_feature;
    if (t->my_actor == NULL && has_flag(FLAG_ACT_CAN_FLY) && t->has_flag(FLAG_TILE_CAN_FLY) && !(feat && feat->has_flag(FLAG_FEAT_NO_FLY))){
        return true;
    }
    return false;
}

// TODO - check whether item is in an inventory?
bool actor::can_take(object * obj) {
    
	vector<object*>::iterator it = inventory->begin();
	
	for(; it != inventory->end(); ++it) {
	
        if (object::can_stack(*it, obj)) {
            return true;
        }
	}

    if(inventory->size() >= MAX_INVENTORY) {
		return false;
	}
    
    return true;
    
}

bool actor::can_eat(object * obj) {

    return obj->get_class()->type == OT_FOOD;
}

bool actor::can_drink(object * obj) {

    return obj->get_class()->type == OT_DRINK;
}

bool actor::can_open(feature * feat) {

    return feat->can_open();
}

bool actor::can_close(feature * feat) {

    return feat->can_close();
}
