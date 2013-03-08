#ifndef TILE_H

#define TILE_H

#include "actor.h"
#include "feature.h"
#include "object.h"

class tile{

	public:
	
	int type;
	object * my_object;
	actor * my_actor;
	feature * my_feature;
	
	tile(short);
	chtype get_img();
	
	bool can_walk();
};

#endif
