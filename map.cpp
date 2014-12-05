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
    event_progress = 0;
    
	tiles = std::vector<std::vector<tile> >(n_width);
    timer_list = new vector<timer *>();
    
	//Create an empty room with walls at the borders
	for(int i = 0; i < width; ++i){
		for(int j = 0; j < height; ++j){
			if( i == 0 || j == 0 || i == width-1 || j == height-1)
				//tiles[i][j] = tile(1 /*TILE_WALL*/);
				tiles[i].push_back(tile(2));
			else
				//tiles[i][j] = tile(0 /*TILE_FLOOR*/);
				tiles[i].push_back(tile(0));
		}
	}
	
}

//===============================================
// Entity Management
//===============================================

void map::add_actor( int x, int y, short code ){
	actor * n = new actor( code );
	put_actor( x, y, n );
}

// TODO - separate out these errors
void map::put_actor( int x, int y, actor * n){

	if( tiles[x][y].my_actor != NULL ) {
		move(2, 2);
		printw("ERROR: add_actor called on tile %d, %d, which already has an actor", x, y);
		getch();
		exit(0);
	}

	n->x = x;
	n->y = y;
	tiles[x][y].my_actor = n;
    n->queue_turn(0);
}

// put something to do this in the actor's destructor
/*void remove_actor( struct actor * act ){
	struct actor * t;
	if(map->act_list == act){
		map->act_list = act->next;
	} else {
		t = map->act_list;
		while(t != NULL && t->next != NULL){ 
			msg(0, 0, 1, t->name);
			getch();
			if( t->next == act ){
				t->next = act->next;
				act->next = NULL;
				return;
			}
			t = t->next;
		}
		fast_msg("Error: tried to remove actor that wasn't in the list\n");
		getch();
	}
}*/

void map::add_object(int x, int y, short code ){
	put_object(x, y, new object(code ));
}

void map::put_object(int x, int y, object * obj) {
    tiles[x][y].my_objects.push_back(obj);
}

void map::add_feature(int x, int y, short code ){
	put_feature(x, y, new feature(code));
}

void map::put_feature( int x, int y, feature * feat){
	tiles[x][y].my_feature = feat;
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