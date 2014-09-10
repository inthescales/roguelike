#include "actor.h"
#include "actclass.h"
#include "globals.h"
#include "map.h"
#include "object.h"
#include "objclass.h"
#include "stringutils.h"
#include "tile.h"
#include "ui.h"
#include "window.h"

#include <algorithm>
#include <sstream>

using std::pair;
using std::ostringstream;

#define SET_SYMBOL(O,S,C) O->img = S|C

actor::actor(short code){

	type = code;
		
	name = aclass[type]->name;
	//new->aitype = aclass[code].ai;
	
	level = 1;
	maxHP = HP = 5;
	gold = 0;
	
	for(int i = 0; i < ES_MAX; ++i){
		equipped_item[i] = NULL;
	}
	
	state = 0; //STATE_STAND;
	
}

d_glyph actor::get_img(){
        return aclass[type]->symbol;
}

int actor::get_color(){
        return aclass[type]->color;
}

string actor::get_name(){
	if(act_player == this)
		return "you";
	else
		return aclass[type]->name;
}

// STATS ================================================

/*
	Returns the current modified stat specified by the input.
	Ideas for later:
		- Store this somewhere to avoid recalculation
		- Add a parameter specifying a code for what types, if any, of modifiers apply
*/
int actor::get_stat(stats_t code, bool always_return){

	// If this is a calculated stat, process that elsewhere
	if (code > CSTAT_MIN )
		return get_calc_stat(code);

	int base, equipment, condition, total;
	
	// Take base stat from actor class
	base = aclass[type]->stats->get_stat(code);
	
	// Check each equipped item and add any stat modifiers provided
	equipment = get_equip_stat(code);
	
	// Check all conditions and add any stat modifiers
	condition = get_cond_stat(code);
	
	total = base + equipment + condition;
	
	return total;
}

int actor::get_stat(stats_t code){
	return get_stat(code, true);
}

int actor::get_equip_stat(stats_t code){

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

int actor::get_cond_stat(stats_t code) {
	
	int ret = 0;
	
	for(int i = 0; i < conditions.size(); ++i){
	
		int value = conditions[i]->get_stat(code);
		if(value != -1) {
			ret += value;
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

// MOVEMENT ==================================================
// and basic attacks

// Move the actor to a new tile
// Assumes the target tile has no actor already.
void actor::move(pair<int,int> offset) {

	tile * old_tile = &map_current->tiles[x][y];
	tile * new_tile = &map_current->tiles[x + offset.first][y + offset.second];

	x += offset.first;
	y += offset.second;
	old_tile->my_actor = NULL;
	new_tile->my_actor = this;

	// If this tile has objects, print a message
	if(!new_tile->my_objects.empty()) {
	
		if(new_tile->my_objects.size() == 1){
		
			object * the_obj = new_tile->my_objects.back();
			if(this == act_player) {
				string out = "You see a " + the_obj->get_name_color() + " here.";
				win_output->print(out);
			}
		} else {
			ostringstream convert;
			convert << new_tile->my_objects.size();
			win_output->print("There are " + convert.str() + " objects here.");
		}
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
	string out = get_name() + color_string(" attack ", C_RED) + target->get_name() + " for " + int_string(damage) + " damage.";
	win_output->print(out);
}

// ITEM INTERACTION ==================================================
// These functions assume that the action can be performed by this actor
// on the specified object. This should be confirmed beforehand.

// Take an object from the ground, if able
bool actor::pick_up(object * target, tile * place){
	
	get_item(target);
	place->remove_object(target);
	
	return true;
}

// Drop an object on the ground
bool actor::drop(object * target, tile * place){
	
	remove_object(target);
	place->my_objects.push_back(target);
	
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
	
	effect * e = item->get_effect(TRG_EAT);
	argmap * m = new argmap();
	m->add_actor(ARG_AGENT, this);
	
	if(e != NULL){
		do_effect(m, e);
	}
}

// Drink an object
bool actor::drink(object * item){
	
	effect * e = item->get_effect(TRG_DRINK);
	argmap * m = new argmap();
	m->add_actor(ARG_AGENT, this);
	
	if(e != NULL){
		//do_effect(new vector<void *>(1, this), e);
	}
}

// Read an object
bool actor::read(object * item){
	
	
}

// Use an object
bool actor::use(object * item){
	
	
}

// SENSES ==================================================

/*void actor::taste(taste_t tst, bool real){

	string msg = "";

	if(real){
		switch(tst){
			
			default:
				msg = "This 
			break;
		}
	} else {
		msg = "You taste "
	}
}*/

// NON-COMMANDS ============================================
// these are not controlled by the user, but handle maintenance
// which may be done automatically or as part of another command.

// Put an item into the actor's inventory, organized by type
void actor::get_item(object * item){
	
	vector<object*>::iterator it = inventory.begin();
	
	for(; it != inventory.end() && (*it)->type < item->type; ++it);
	
	if(this == act_player){
		char c = UI::get_next_letter();
	
		item->letter = c;
		obj_letter[UI::letter_to_int(c)] = item;
	}
	
	inventory.insert(it, item);
}

// Remove an object from the actor's inventory
bool actor::remove_object(object * item){
	
	obj_letter[UI::letter_to_int(item->letter)] = NULL;
	item->letter = 0;
	vector<object*>::iterator it = std::find( inventory.begin(), inventory.end(), item);
	inventory.erase(it);
	
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

// Effect management =========================

effect * actor::get_effect(trigger_t trigger){

	for(int i = 0; i < aclass[type]->effects.size(); ++i){
		
		if(aclass[type]->effects[i].trigger == trigger)
			return &(aclass[type]->effects[i].eff);
	}
	
	return NULL;
}

void actor::resolve_trigger(trigger_t trigger, argmap * args) {
	
	effect * my_effect = get_effect(trigger);
	argmap * my_map = new argmap();
	my_map->add_actor(ARG_HOLDER_ACTOR, this);
	if (args != NULL) {
		my_map->add_args(args);
	}
	
	if (my_effect != NULL) {
		do_effect(my_map, my_effect);
	}
	
	vector<condition*>::iterator cit = conditions.begin();
	for(; cit != conditions.end(); ++cit) {
	
		(*cit)->resolve_trigger(trigger, my_map);
	}
	
	vector<object*>::iterator oit = inventory.begin();
	for(; oit != inventory.end(); ++oit) {
	
		(*oit)->resolve_trigger(trigger, my_map);
	}
}

// Condition management =========================

bool actor::has_condition(int code) {

	vector<condition*>::iterator it = conditions.begin();
	
	for(; it != conditions.end(); ++it) {
		
		if ((*it)->type == code) {
			return true;
		}
	}
	
	return false;
}

condition * actor::get_condition(int code) {

	vector<condition*>::iterator it = conditions.begin();
	
	for(; it != conditions.end(); ++it) {
		
		if ((*it)->type == code) {
			return *it;
		}
	}
	
	return NULL;
}

bool actor::add_condition(condition * cond) {

	vector<condition*>::iterator it = conditions.begin();
	
	for(; it != conditions.end(); ++it) {
		
		if ((*it)->type == cond->type) {
		
			(*it)->add_condition(cond);
			return true;
		}
	}
	
	conditions.push_back(cond);
	return false;
}

bool actor::remove_condition(int code) {

	vector<condition*>::iterator it = conditions.begin();
	
	for(; it != conditions.end(); ++it) {
		
		if ((*it)->type == code) {
			conditions.erase(it);
			return true;
		}
	}
	
	return false;
}

// Process decay on my conditions, and my inventory's
void actor::decay_conditions() {

	vector<condition*>::iterator it = conditions.begin();
	for(; it != conditions.end(); ++it) {
		
		if(!(*it)->do_decay()) {
			condition * dead_cond = (*it);
			it = conditions.erase(it);
			delete(dead_cond);
		}
	}

	vector<object*>::iterator it2 = inventory.begin();
	for(; it2 != inventory.end(); ++it2) {
		
		(*it2)->decay_conditions();
	}
}
