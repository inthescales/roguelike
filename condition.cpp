#include "condition.h"
#include "condclass.h"
#include "enums.h"
#include "globals.h"
#include "map.h"

#include <algorithm>

using std::max;
using std::min;

condition::condition(int ntype) {
	
	type = ntype;
	stack = 1;
	duration = cclass[type]->base_duration;
    start_timers();
}

condition::condition(int ntype, int ntime) {

	type = ntype;
	stack = 1;
	duration = ntime;
    start_timers();
}

condition::condition(int ntype, int nstack, int ntime) {

	type = ntype;
	stack = nstack;
	duration = ntime;
    start_timers();
}

void condition::destroy() {

    
    destroy_timers();
}

void condition::start_timers() {

    // Create timeout timer
    if (duration != -1) {
        effect * kill_effect = new effect(EFF_COND_TIMEOUT);
        argmap * kill_args = new argmap();
        kill_args->add_condition(ARG_HOLDER_CONDITION ,this);
        map_current->add_timer(new timer(kill_effect, kill_args, duration, 0, 0));
    }
    
    // Create timers for timed effects in the condition's class
    std::vector<timer_effect>::iterator it = cclass[type]->timed_effects.begin();
    for (;it != cclass[type]->timed_effects.end(); ++it) {
        argmap * newmap = new argmap();
        newmap->add_condition(ARG_HOLDER_CONDITION, this);
        map_current->add_timer(new timer(it->eff, newmap, it->time, it->iterations, it->delta));
    }
}

void condition::destroy_timers() {
    
    vector<timer *>::iterator it = timer_list->begin();
    
    for(; it != timer_list->end(); ++it) {
    
        map_current->remove_timer(*it);
    }
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
        case CSPOL_STACK: stack += other->stack; return;
		case CSPOL_SET  : strength = other->strength; return;
		case CSPOL_ADD  : strength += other->strength; return;
		case CSPOL_MAX  : strength = max(strength, other->strength); return;
		case CSPOL_MIN  : strength = min(strength, other->strength); return;
		case CSPOL_IGNORE:
		default:
			return;
	}

}

/* 
 Process condition deltas, described in condition class
 Returns whether it is possible for the condition to continue.
*/
bool condition::do_decay() {

    stack += cclass[type]->stack_delta;
    if (stack > cclass[type]->max_stack) {
        stack = cclass[type]->max_stack;
    }
    if (stack < 0) {
        return false;
    }
    
    strength += cclass[type]->strength_delta;
    if (strength < 0) {
        return false;
    }
    
    return true;
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
	return get_stat(code) * (stack * cclass[type]->stack_stats);
}

int condition::get_modified_strength() {
	return strength * (stack * cclass[type]->stack_strength);
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
    
    if (my_effect != NULL) {
        argmap * my_map = new argmap();
        my_map->add_condition(ARG_HOLDER_CONDITION, this);

        if (args != NULL) {
            my_map->add_args(args);
        }
	
		do_effect(my_map, my_effect);
	}
}
