#ifndef OBJCLASS_H

#define OBJCLASS_H

#include "enums.h"
#include "effect.h"

#include <utility>
#include <string>
#include <vector>

using std::string;
using std::vector;

class objclass {

	public:
	
	objclass(string, unsigned char, colorName, object_type, object_subtype, material_t, int, int);
			 
	void add_effect(trigger_effect);
	
	string name;
	string assigned_name; //user assigned name
	
	unsigned char symbol;
	colorName color;
	
	bool known;
	
	object_type type;
	object_subtype subtype;
	std::vector<trigger_effect> effects;
	
	material_t material;
	int weight;
	int value;
	
};

#endif