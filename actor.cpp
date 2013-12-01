#include "actor.h"
#include "actclass.h"
#include "globals.h"
#include "graphics.h"
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
		
	name = aclass[type].name;
	//new->aitype = aclass[code].ai;
	
	level = 1;
	maxHP = HP = 5;
	gold = 0;
	
	for(int i = 0; i < ES_MAX; ++i){
		equipped_item[i] = NULL;
	}
	
	state = 0; //STATE_STAND;
	
}

chtype actor::get_img(){
	return comp(aclass[type].symbol, aclass[type].color);
}

string actor::get_name(){
	if(act_player == this)
		return "you";
	else
		return aclass[type].name;
}

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
	string out = get_name() + color_string(" attack ", C_RED) + target->get_name() + ".";
	win_output->print(out);
}

// Take an object from the ground, if able
bool actor::pick_up(object * target, tile * place){
	
	get_item(target);
	place->remove_object(target);
	
	return true;
}

bool actor::drop(object * target, tile * place){
	
	remove_object(target);
	place->my_objects.push_back(target);
	
	return true;
}

bool actor::equip(object * item, int slot){

	equipped_item[slot] = item;
	item->equipped = 1;
}

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

bool actor::eat(object * item){
	
	effect * e = item->get_effect(TRG_EAT);
	argmap * m = new argmap();
	m->add_actor(ARG_AGENT, this);
	
	if(e != NULL){
		do_effect(m, e);
	}
}

bool actor::drink(object * item){
	
	effect * e = item->get_effect(TRG_DRINK);
	argmap * m = new argmap();
	m->add_actor(ARG_AGENT, this);
	
	if(e != NULL){
		//do_effect(new vector<void *>(1, this), e);
	}
}

bool actor::read(object * item){
	
	
}

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

bool actor::remove_object(object * item){
	
	obj_letter[item->letter] = NULL;
	item->letter = 0;
	vector<object*>::iterator it = std::find( inventory.begin(), inventory.end(), item);
	inventory.erase(it);
	
	return true;
}

void actor::print(string a, string b){
	
	if(this == act_player)
		win_output->print(a);
	else
		win_output->print(b);
}
