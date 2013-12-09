#ifndef OBJCLASS_H

#define OBJCLASS_H

#include "enums.h"
//#include "effect.h"

#include <utility>
#include <string>
#include <vector>

class argmap;
typedef argmap statmap;
class effect;
class trigger_effect;

using std::string;
using std::vector;

class objclass {

	public:

	string name;
	string assigned_name; //user assigned name
	
	unsigned char symbol;
	colorName color;
	
	bool known;
	
	object_type type;
	object_subtype subtype;
	vector<trigger_effect> effects;
	statmap * stats;
	
	material_t material;
	int weight;
	int value;
	
	objclass(string, unsigned char, colorName, object_type, object_subtype, material_t, int, int);	
	void add_effect(trigger_effect);
	
};

#endif