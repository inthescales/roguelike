#ifndef OBJCLASS_H

#define OBJCLASS_H

#include "display.h"
#include "enums.h"

#include <utility>
#include <string>
#include <vector>

using std::string;
using std::vector;

class argmap;
class trigger_effect;

typedef argmap statmap;

class objclass {

	public:

	string name;
	string assigned_name; //user assigned name
	
	d_glyph symbol;
	colorName color;
	
	bool known;
	
	object_type type;
	object_subtype subtype;
	
	material_t material;
	int weight;
	int value;
	
	vector<trigger_effect> effects;
	statmap * stats;
	
	objclass(string, d_glyph, colorName, object_type, object_subtype, material_t, int, int);	
	void add_effect(trigger_effect);
	
};

#endif
