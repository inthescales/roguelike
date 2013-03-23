#ifndef TILE_H

#define TILE_H

#include "actor.h"
#include "feature.h"
#include "object.h"

#include <vector>

using namespace std;

class tile{

	public:
	
	int type;
	actor * my_actor;
	feature * my_feature;
	vector<object> my_objects;
	
	tile(short);
	chtype get_img();
	
	bool can_walk();
};

#endif
