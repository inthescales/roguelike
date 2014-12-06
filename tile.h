#ifndef TILE_H

#define TILE_H

#include "display.h"
#include "glyph.h"
#include "mapentity.h"

#include <vector>

using std::vector;

class actor;
class condition;
class object;
class feature;

class tile : public mapentity {

	public:

	actor * my_actor;
	feature * my_feature;
	vector<object*> * my_objects;
	
	//Functions:
	tile(short);
    void init();
	
    tileclass * get_class();
    glyph get_display_glyph();
	bool can_walk();
	
    bool get_object(object *);
	bool remove_object(object *);
};

#endif
