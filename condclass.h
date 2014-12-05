#ifndef CONDCLASS_H

#define CONDCLASS_H

#include "effect.h"
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
    
    int stack_delta;
	int strength_delta;
	int duration_delta; // will be passed into timer
    int iterations; // will be passed into timer
    
    bool stack_stats;
    bool stack_strength;
	
    int max_stack;
	cond_stack_policy_t stack_policy;
	cond_timer_policy_t timer_policy;
	
	vector<trigger_effect> effects;
    vector<timer_effect> timed_effects;
	statmap * stats;
	
	condclass(string, colorName, int, int, int, int, int, int, bool, bool, int, cond_stack_policy_t, cond_timer_policy_t);
	void add_effect(trigger_effect);
};

#endif