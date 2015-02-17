#include "actor.h"
#include "color.h"
#include "object.h"
#include "feature.h"
#include "globals.h"
#include "tile.h"
#include "tileclass.h"

#include <algorithm>
#include <math.h>
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
    
	if(my_actor != NULL && seen){
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
        ret.color = color::get_grayed(ret.color);
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

// Class methods ===================================

vector<tile*> * tile::line_between(tile * orig, tile * dest) {

    if(orig->current_map != dest->current_map) {
        // Something is horribly wrong
        return NULL;
    }
    
    map * m = orig->current_map;
    
    int x1 = orig->x, x2 = dest->x, y1 = orig->y, y2 = dest->y;
    vector<tile*> * ret = new vector<tile*>();
    
#if LINE_ALGORITHM == LINE_BRESENHAM
    // Bresenham's line algorithm
    
    const bool steep = (fabs(y2 - y1) > fabs(x2 - x1));
    float error;
    const float dx = fabs(x2 - x1);
    const float dy = fabs(y2 - y1);
    const int xstep = (x1 < x2) ? 1 : -1;
    const int ystep = (y1 < y2) ? 1 : -1;

    if (!steep) {
        error = dx / 2.0f;
        int y = (int)y1;
        const int xend = (x1 < x2) ? x2+1 : x2-1;
        for(int x=(int)x1; x != xend; x += xstep)
        {
        
            if(!(x == x1 && y == y1)) {
                ret->push_back(&m->tiles[x][y]);
            }

            error -= dy;
            if(error < 0)
            {
                y += ystep;
                error += dx;
            }

        }
    } else {
        error = dy / 2.0f;
        int x = (int)x1;
        const int yend = (y1 < y2) ? y2+1 : y2-1;
        for(int y=(int)y1; y != yend; y += ystep)
        {
        
            if(!(x == x1 && y == y1)) {
                ret->push_back(&m->tiles[x][y]);
            }

            error -= dx;
            if(error < 0)
            {
                x += xstep;
                error += dy;
            }

        }
    }
  
#endif

    return ret;

}

