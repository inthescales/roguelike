#include "globals.h"
#include "map.h"
#include "tile.h"
#include "feature.h"
#include "window.h"

map::map(short n_width, short n_height, int n_type, int n_id, map * last_map) : width(n_width), height(n_height), id(n_id){

	tiles = std::vector<std::vector<tile> >(n_width);

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

void map::add_actor( int x, int y, short code ){
	actor * n = new actor( code );
	put_actor( x, y, n );
}

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
	actors.push_back(n);
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

void map::add_object( int x, int y, short code ){
	tiles[x][y].my_objects.push_back( object( code ) );
}

void map::add_feature( int x, int y, short code ){
	tiles[x][y].my_feature = new feature( code );
}
