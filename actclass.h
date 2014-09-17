#ifndef ACTCLASS_H

#define ACTCLASS_H

#include "argmap.h"
#include "effect.h"
#include "enums.h"
#include "glyph.h"

#include <string>

using std::string;

class actclass {

	public:
	
	std::string name;
	glyph image;
	short ai;
	
	vector<trigger_effect> effects;
	statmap * stats;
	
	actclass(string, symbol_code, colorName);
	void add_effect(trigger_effect n);
};

#endif
