#ifndef OBJCLASS_H

#define OBJCLASS_H

#include "display.h"
#include "enums.h"
#include "glyph.h"

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
	
	glyph image;
	
	bool known;
	
	object_type type;
	object_subtype subtype;
	
	material_t material;
	int weight;
	int value;
	
	vector<trigger_effect> effects;
	statmap * stats;
	
	objclass(string, symbol_code, colorName, object_type, object_subtype, material_t, int, int);	
	
	glyph get_glyph();
    symbol_code get_symbol();
	colorName get_color();
    
    void add_effect(trigger_effect);
};

#endif
