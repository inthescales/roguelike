#include <stdio.h>
#include <stdlib.h>
#include "tile.h"
#include "globals.h"
#include "graphics.h"

tile::tile(short code){
	type = code;

	my_object = NULL;
	my_actor = NULL;
	my_feature = NULL;
}

chtype tile::get_img(){
	if(my_actor != NULL){
		return comp(aclass[my_actor->type].symbol, aclass[my_actor->type].color);
	} else if(my_feature != NULL){
	
	} else if(my_object != NULL){
	
	} else {
		return comp(tclass[type].symbol, tclass[type].color);
	}
}

bool tile::can_walk(){
	return tclass[type].can_walk;
}