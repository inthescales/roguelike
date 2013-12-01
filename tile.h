#ifndef TILE_H

#define TILE_H

#include "actor.h"
#include "condition.h"
#include "feature.h"
#include "object.h"

#include <vector>

using std::vector;

class tile{

	public:
	
	int type;
	actor * my_actor;
	feature * my_feature;
	vector<object*> my_objects;
	condition * conditions;
	
	//Functions:
	tile(short);
	
	// Fancy getters
	bool can_walk();
	
	chtype get_img();
	bool remove_object(object*);
};

#endif
