#ifndef TILE_H

#define TILE_H

#include "display.h"

#include <vector>

using std::vector;

class actor;
class condition;
class object;
class feature;
class glyph;

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
	
	glyph get_img();
	int get_img_color();
	glyph get_glyph();
	int get_color();
	bool remove_object(object*);
};

#endif
