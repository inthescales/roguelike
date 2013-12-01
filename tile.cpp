#include <stdio.h>
#include <stdlib.h>
#include "tile.h"
#include "globals.h"
#include "graphics.h"
#include "tileclass.h"

#include <algorithm>

tile::tile(short code){
	type = code;

	my_actor = NULL;
	my_feature = NULL;
}

chtype tile::get_img(){
	if(my_actor != NULL){
		my_actor->get_img();
	} else if(my_feature != NULL){
	
	} else if(!my_objects.empty()){
		my_objects.back()->get_img();
	} else {
		return comp(tclass[type].symbol, tclass[type].color);
	}
}

bool tile::can_walk(){
	
	return tclass[type].can_walk;
}

// Remove an object from the tile's list (because it's been taken)
bool tile::remove_object(object * in){
	
	vector<object*>::iterator it = std::find( my_objects.begin(), my_objects.end(), in);
	my_objects.erase(it);
	
	return true;
}
