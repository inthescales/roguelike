#include "actor.h"
//#include "ai.h"
//#include "feature.h"
//#include "featclass.h"
#include "globals.h"
#include "graphics.h"
//#include "load.h"
#include "map.h"
#include "object.h"
#include "objclass.h"
#include "stringutils.h"
#include "tile.h"
//#include "tileclass.h"
#include "window.h"

#include <algorithm>

using namespace std;

#define SET_SYMBOL(O,S,C) O->img = S|C

actor::actor(short code){

	type = code;
		
	name = aclass[type].name;
	//new->aitype = aclass[code].ai;
	
	level = 1;
	maxHP = HP = 5;
	gold = 0;
	
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
				string out = "You see a " + color_string(the_obj->get_name(), oclass[the_obj->type].color) + " here.";
				win_output->print(out);
			}
		} else {
			win_output->print("There's a bunch of junk here.");
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

// NON-COMMANDS ============================================

// Put an item into the actor's inventory, organized by type
void actor::get_item(object * item){
	
	vector<object*>::iterator it = inventory.begin();
	
	for(; it != inventory.end() && (*it)->type > item->type; ++it);
	
	inventory.insert(it, item);
}

bool actor::remove_object(object * item){
	
	vector<object*>::iterator it = std::find( inventory.begin(), inventory.end(), item);
	inventory.erase(it);
	
	return true;
}
