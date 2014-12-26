#ifndef OBJECT_H
#define OBJECT_H

#include "argmap.h"
#include "config.h"
#include "display.h"
#include "effect.h"
#include "enums.h"
#include "mapentity.h"

#include <utility>
#include <string>
#include <vector>

using std::string;
using std::vector;

class condition;

class object : public mapentity {

	public:

    string individual_name;
	short letter;
	int quantity;
	bool equipped; //TODO - change to flag
	
	//Functions:
	object(short);
	object(short, short);
    void init();
    
    objclass * get_class();
    string get_name();
    string get_name_color();
    object_type get_type();
    object_subtype get_subtype();
    
	static bool compare_type(object *, object *);
    static bool can_stack(object *, object *);
    
	void resolve_trigger(trigger_t, argmap *);
};

#endif
