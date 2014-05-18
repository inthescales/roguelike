#ifndef CONDCLASS_H

#define CONDCLASS_H

#include "enums.h"
#include <string>
#include <vector>

class argmap;
class trigger_effect;

typedef argmap statmap;

using std::string;
using std::vector;

class condclass {

	public:
	string name;
	colorName color;
	
	int base_strength;
	int base_duration;
	
	int decay_interval;
	int decay_value;
	
	cond_stack_policy_t stack_policy;
	cond_timer_policy_t timer_policy;
	
	vector<trigger_effect> effects;
	statmap * stats;
	
	condclass(string, colorName, int, int, int, int, cond_stack_policy_t, cond_timer_policy_t);
	void add_effect(trigger_effect);
};

#endif