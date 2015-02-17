#include "map.h"

#include "actor.h"
#include "effect.h"
#include "feature.h"
#include "globals.h"
#include "object.h"
#include "tile.h"
#include "timer.h"
#include "window.h"

#include <iterator>

map::map(short n_width, short n_height, int n_id, map * last_map) {

    width = n_width;
    height = n_height;
    id = n_id;
    
	tiles = std::vector<std::vector<tile> >(n_width);
    timer_list = new vector<timer *>();
    
	//Create an empty room with walls at the borders
	for(int i = 0; i < width; ++i){
		for(int j = 0; j < height; ++j){
			if( i == 0 || j == 0 || i == width-1 || j == height-1) {
				tiles[i].push_back(tile(2));
			} else {
				tiles[i].push_back(tile(0));
            }
            tiles[i].back().x = i;
            tiles[i].back().y = j;
            tiles[i].back().current_map = this;
		}
	}
	
    //tiles[20][20] = tile(3);
}

//===============================================
// Entity Management
//===============================================

void map::add_actor( int x, int y, short code ){
	actor * n = new actor( code );
	put_actor( x, y, n );
}

// TODO - separate out these errors
void map::put_actor( int x, int y, actor * act){

	if( tiles[x][y].my_actor != NULL ) {
		move(2, 2);
		printw("ERROR: add_actor called on tile %d, %d, which already has an actor", x, y);
		getch();
		exit(0);
	}

    tiles[x][y].my_actor = act;
	act->x = x;
	act->y = y;
    act->current_map = this;
    act->queue_turn(0);
}

void map::add_object(int x, int y, short code ){
	put_object(x, y, new object(code ));
}

void map::put_object(int x, int y, object * obj) {
    tiles[x][y].my_objects->push_back(obj);
    obj->x = x;
    obj->y = y;
    obj->current_map = this;
}

void map::add_feature(int x, int y, short code ){
	put_feature(x, y, new feature(code));
}

void map::put_feature( int x, int y, feature * feat){
	tiles[x][y].my_feature = feat;
    feat->x = x;
    feat->y = y;
    feat->current_map = this;
}

//===============================================
// Timers and Timed Events
//===============================================

void map::add_timer(timer * n_timer) {

    long ttp = 0;
    vector<timer *>::iterator it = timer_list->begin();
    
    // Find the last event before the new one, and count the ticks until that point
    for(; it != timer_list->end(); ++it) {

        int nextVal;
        if (it == timer_list->begin()) nextVal = time_until_event();
        else nextVal = (*it)->time;

        if (n_timer->time < ttp + nextVal) break;
        
        ttp += nextVal;
    }

    it = timer_list->insert(it, n_timer);

    // Change new timer's time
    if (it != timer_list->begin()) {
        n_timer->time = n_timer->time - ttp;
    }

    // Change next timer's time
    if (it != timer_list->end() && ++it != timer_list->end()) {
        timer * next_timer = (*it);
        next_timer->time -= n_timer->time;
    }
}

void map::remove_timer(timer * o_timer) {
    
    vector<timer *>::iterator it = timer_list->begin();

    // Find the last event before the specified one, and count the ticks until that point
    for(; it != timer_list->end() && (*it) != o_timer; ++it);
    
    if (it != timer_list->end()) {
    
        int o_time = (*it)->time;
        it = timer_list->erase(it);
        
        if (it != timer_list->end()) {
            (*it)->time += o_time;
        }
    }
}

void map::advance_time() {

    if (timer_list->empty()) {
        return;
    }
    
    timer * current = timer_list->at(0);
    timer_list->erase(timer_list->begin());
    
    bool can_continue = current->execute();

    if (can_continue) {
        timer * new_timer = current->create_child();
        
        if (new_timer != NULL) {
            add_timer(new_timer);
        }
    }
}

int map::time_until_event() {

    if (!timer_list->empty()) {
        return timer_list->at(0)->time;
    }

    return -1;
}