#ifndef CONDITION_H

#define CONDITION_H

#include "effect.h"
#include "argmap.h"

class condition {

	public:
	
	int type;
	int stack;
	int duration;
	
	double time_mod;
	double strength_mod;
	
	//Functions:
	condition(int);
	condition(int, int);
	condition(int, int, int);
	
	void add_condition(condition *);
	bool do_decay();
	
	bool has_stat(stats_t);
	int get_stat(stats_t);
	int get_modified_stat(stats_t);
	int get_modified_strength();
	
	effect * get_effect(trigger_t);
	void resolve_trigger(trigger_t, argmap *);
};

#endif