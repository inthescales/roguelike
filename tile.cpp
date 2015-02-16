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
    individual_name = "";
	my_actor = NULL;
	my_feature = NULL;
    
    init();
}

void tile::init() {

    mapentity::init();
    
    known = seen = false;
    my_objects = new vector<object *>;
}

// Basic information ============================

tileclass * tile::get_class() {
    return (tileclass *)my_class;
}

glyph tile::get_display_glyph() {

    glyph ret;
    
    if (!known) {
        // If we've never seen this tile, print a black tile
        ret.symbol = symboldef[SYM_UNKNOWN];
        ret.color = C_BLACK;
        return ret;
    }
    
	if(my_actor != NULL){
		ret = my_actor->get_glyph();
	} else if(my_feature != NULL){
        ret = my_feature->get_glyph();
	} else if(!my_objects->empty()){
		ret = my_objects->back()->get_glyph();
	} else {
	    ret = get_glyph();
	}
    
    if (!seen) {
        // If we've seen this tile but it's not in view, print in gray
        ret.color = C_GRAY;
    }
    
    return ret;
}

// Common flags ===========================

bool tile::is_opaque() {
    bool ret = (has_flag(FLAG_TILE_OPAQUE) ||
                (my_feature != NULL && my_feature->is_opaque()) ||
                (my_actor != NULL && my_actor->is_opaque()) );
    return ret;
}

// Contents management ===========================

bool tile::get_object(object * in) {
    
    in->x = x;
    in->y = y;
    my_objects->push_back(in);
    
    return true;
}

// Remove an object from the tile's list (because it's been taken)
bool tile::remove_object(object * in){
	
	vector<object*>::iterator it = std::find( my_objects->begin(), my_objects->end(), in);
	my_objects->erase(it);
	
	return true;
}
