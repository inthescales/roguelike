#ifndef OBJCLASS_H

#define OBJCLASS_H

#include "enums.h"

#include <string>

class objclass {

	public:
	
	std::string name;
	std::string assignedName; //user assigned name
	
	unsigned char symbol;
	colorName color;
	
	bool known;
	
	object_type type;
	short material;
	int weight;
	int value;
	
};

#endif