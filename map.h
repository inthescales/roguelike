#ifndef MAP_H

#define MAP_H

#include <vector>

#include "tile.h"

/*
	When I'm creating a room, sometimes rooms will want to propagate
	to the next floor down. This stores those reminders until the
	map is actually generated.
	
	SAVE THIS FOR LATER

struct roomNote {
	int x, y;
	int index, type;
	struct roomNote * next;
};
*/

class map{

	public:

	map(short, short, int, int, struct map *);
	void add_stairs( struct feature *, int, int);
	void add_actor( int, int, short );
	void put_actor( int, int, struct actor *);
	void add_object( int, int, short );
	void put_object( int, int, short);
	void add_feature( int, int, short );
	
	int id;
	short width, height;
	std::vector<std::vector<tile> > tiles;
	//std::vector<stair> stairs;
	std::vector<actor *> actors;
	//struct roomNote * note;
};

#endif
