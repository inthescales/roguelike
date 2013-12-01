#ifndef OBJECT_H

#define OBJECT_H

//#include "condition.h"
#include "effect.h"
#include "enums.h"

#include "curses.h"
#include <utility>
#include <string>
#include <vector>

using std::string;
using std::vector;

class condition;

class object {

	public:

	short letter;
	int quantity;
	int type;
	bool equipped; //TODO - not totally happy with this...
	condition * conditions;
	
	//Functions:
	object(short);
	object(short, short);
	
	static bool compare_type(object *, object *);
	
	chtype get_img();
	string get_name();
	string get_name_color();
	
	bool has_trigger(trigger_t);
	effect * get_effect(trigger_t);
};

#endif
