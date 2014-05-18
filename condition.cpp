#include "condition.h"
#include "condclass.h"
#include "globals.h"

#include <algorithm>

using std::max;
using std::min;

condition::condition(int ntype, int nstack) {

	type = ntype;
	stack = nstack;
	duration = cclass[type]->base_duration;
	
	time_mod = strength_mod = 1;
}

condition::condition(int ntype, int nstack, int ntime) {

	type = ntype;
	stack = nstack;
	duration = ntime;
	
	time_mod = strength_mod = 1;
}

/*
 Handles adding a condition to a holder that already has a condition
 of the same type. Policies are used here.
*/
void condition::add_condition(condition * other) {

	switch(cclass[type]->timer_policy) {
		case CTPOL_SET : duration = other->duration; return;
		case CTPOL_ADD : duration += other->duration; return;
		case CTPOL_MAX : duration = max(duration, other->duration); return;
		case CTPOL_MIN : duration = min(duration, other->duration); return;
		case CTPOL_IGNORE:
		default:
			return;
	}
	
	switch(cclass[type]->stack_policy) {
		case CSPOL_SET : stack = other->stack; return;
		case CSPOL_ADD : stack += other->stack; return;
		case CSPOL_MAX : stack = max(stack, other->stack); return;
		case CSPOL_MIN : stack = min(stack, other->stack); return;
		case CSPOL_IGNORE:
		default:
			return;
	}

}

/* 
 For effects that slowly decay, this resolves that decay.
 Reduce the stack / strengh by the decay value, and reset
 its time to the 
*/
void condition::do_decay() {

	duration--;
	
	if (stack > 0 && duration % (int)(cclass[type]->decay_interval * time_mod) == 0) {
		stack -= cclass[type]->decay_value;
	}
	
	// If duration <= 0 || stack <= 0, destroy self
}

// STATS =========================

bool condition::has_stat(stats_t code){

	return cclass[type]->stats->has_stat(code);
}

int condition::get_stat(stats_t code){

	int value = 0;

	if (has_stat(code)){
	
		value = cclass[type]->stats->get_stat(code);
	}
	
	return value;
}

int condition::get_modified_stat(stats_t code) {
	return get_stat(code) * stack * strength_mod;
}

int condition::get_modified_strength() {
	return cclass[type]->base_strength * stack * strength_mod;
}

// EFFECTS ============================================

effect * condition::get_effect(trigger_t trigger){

	for(int i = 0; i < cclass[type]->effects.size(); ++i){
		
		if(cclass[type]->effects[i].trigger == trigger) return &(cclass[type]->effects[i].eff);
	}
	
	return NULL;
}

void condition::resolve_trigger(trigger_t trigger, argmap * args) {
	
	effect * my_effect = get_effect(trigger);
	argmap * my_map = new argmap();
	my_map->add_int(ARG_COND_STR, get_modified_strength());
	if (args != NULL) {
		my_map->add_args(args);
	}
	
	if (my_effect != NULL) {
		do_effect(my_map, my_effect);
	}
}
