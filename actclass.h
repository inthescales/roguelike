#ifndef ACTCLASS_H

#define ACTCLASS_H

#include "argmap.h"
#include "effect.h"
#include "enums.h"

#include <string>

using std::string;

class actclass {

	public:
	
	std::string name;
	unsigned char symbol;
	colorName color;
	short ai;
	
	vector<trigger_effect> effects;
	statmap * stats;
	
	actclass(string, unsigned char, colorName);
	void add_effect(trigger_effect n);
};

#endif