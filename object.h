#ifndef OBJECT_H

#define OBJECT_H

#include "argmap.h"
#include "config.h"
#include "display.h"
#include "effect.h"
#include "enums.h"

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
	vector<condition*> conditions;
	
	//Functions:
	object(short);
	object(short, short);
	
	static bool compare_type(object *, object *);
	
	d_glyph get_img();
	int get_color();
	string get_name();
	string get_name_color();
	
	bool has_stat(stats_t);
	int get_stat(stats_t);
	int get_stat(stats_t, bool);
	int get_cond_stat(stats_t);
	
	bool has_trigger(trigger_t);
	effect * get_effect(trigger_t);
	void resolve_trigger(trigger_t, argmap *);
	
	bool has_condition(int);
	condition * get_condition(int);
	bool add_condition(condition *);
	bool remove_condition(int);
	void decay_conditions();
};

#endif
