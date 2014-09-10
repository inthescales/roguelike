#ifndef TILE_H

#define TILE_H

#include "display.h"

#include <vector>

using std::vector;

class actor;
class object;
class feature;
class condition;

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
	
	d_glyph get_img();
	int get_color();
	bool remove_object(object*);
};

#endif
