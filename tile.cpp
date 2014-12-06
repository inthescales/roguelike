#include "actor.h"
#include "object.h"
#include "feature.h"
#include "globals.h"
#include "tile.h"
#include "tileclass.h"

#include <algorithm>
#include <stdio.h>
#include <stdlib.h>

// Setup and teardown ======================

tile::tile(short code){

    my_class = (tileclass *)tclass[code];
	my_actor = NULL;
	my_feature = NULL;
    
    init();
}

void tile::init() {

    mapentity::init();
    
    my_objects = new vector<object *>;
}

// Basic information ============================

tileclass * tile::get_class() {
    return (tileclass *)my_class;
}

glyph tile::get_display_glyph() {

	if(my_actor != NULL){
		my_actor->get_glyph();
	} else if(my_feature != NULL){
        my_feature->get_glyph();
	} else if(!my_objects->empty()){
		my_objects->back()->get_glyph();
	} else {
	    get_glyph();
	}
}

bool tile::can_walk(){
	
	return get_class()->can_walk;
}

// Contents management ===========================

bool tile::get_object(object * in) {
    
    my_objects->push_back(in);
    
    return true;
}

// Remove an object from the tile's list (because it's been taken)
bool tile::remove_object(object * in){
	
	vector<object*>::iterator it = std::find( my_objects->begin(), my_objects->end(), in);
	my_objects->erase(it);
	
	return true;
}
