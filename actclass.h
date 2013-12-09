#ifndef ACTCLASS_H

#define ACTCLASS_H

#include "argmap.h"
#include "enums.h"

#include <string>

using std::string;

class actclass {

	public:
	
	std::string name;
	unsigned char symbol;
	colorName color;
	
	statmap * stats;
	short ai;
	
	//-----
	actclass(string, unsigned char, colorName);
	void add_basic_stats(int, int);
	
};

#endif