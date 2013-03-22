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

void actor::move(pair<int,int> offset) {

	map_current->tiles[x][y].my_actor = NULL;
	
	x += offset.first;
	y += offset.second;
	
	map_current->tiles[x][y].my_actor = this;
	
	
}

void actor::attack(pair<int,int> offset) {
	tile * place;
	place = &map_current->tiles[x + offset.first][y + offset.second];
	attack(place->my_actor);
}

void actor::attack(actor * target){
	string out = get_name() + color_string(" attack ", C_RED) + target->get_name();
	win_output->print(out);
}