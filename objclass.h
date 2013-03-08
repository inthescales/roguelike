#ifndef OBJCLASS_H

#define OBJCLASS_H

#include "enums.h"

#include <string>

class objclass {

	std::string name;
	std::string assignedName; //user assigned name
	
	unsigned char symbol;
	colorName color;
	
	bool known;
	
	short type;
	short subtype;
	short material;
	int weight;
	int value;
	
	//std::vector <effect> effects;

};

#endif