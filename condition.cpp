#include "condition.h"
#include "condclass.h"
#include "enums.h"
#include "map.h"
#include "mapentity.h"
#include "rogue.h"
#include "stringutils.h"

#include <algorithm>

using std::max;
using std::min;

// Setup and teardown ============================

condition::condition(mapentity * n_host, int ntype) {
	
    my_class = (entityclass *)cclass[ntype];
    individual_name = "";
	stack = 1;
	duration = get_class()->base_duration;
    host = n_host;

    init();
}

condition::condition(mapentity * n_host, int ntype, int ntime) {

    my_class = (entityclass *)cclass[ntype];
    individual_name = "";
	stack = 1;
	duration = ntime;
    host = n_host;

    init();
}

condition::condition(mapentity * n_host, int ntype, int nstack, int ntime) {

    my_class = (entityclass *)cclass[ntype];
    individual_name = "";
	stack = nstack;
	duration = ntime;
    host = n_host;

    init();
}

void condition::init() {

    entity::init();
    start_timers(host->current_map);
}

void condition::destroy() {
    destroy_timers(host->current_map);
}

void condition::start_timers(map * n_map) {
    
    entity::start_timers(n_map);
    
    // Create timeout timer
    if (duration != -1) {
        effect * kill_effect = new effect(EFF_COND_TIMEOUT);
        argmap * kill_args = new argmap();
        kill_args->add_condition(ARG_HOLDER_CONDITION ,this);
        map_current->add_timer(new timer(kill_effect, kill_args, duration, 0, 0));
    }
}

// Basic info ========================

condclass * condition::get_class() {
    return (condclass *)entity::get_class();
}

colorName condition::get_color(){
    return get_class()->color;
}

string condition::get_name_color(){
    return color_string(get_name(), get_color());
}

// Condition mechanics ================================

/*
 Handles adding a condition to a holder that already has a condition
 of the same type. Policies are used here.
*/
void condition::add_condition(condition * other) {

	switch(get_class()->timer_policy) {
		case CTPOL_SET : duration = other->duration; return;
		case CTPOL_ADD : duration += other->duration; return;
		case CTPOL_MAX : duration = max(duration, other->duration); return;
		case CTPOL_MIN : duration = min(duration, other->duration); return;
		case CTPOL_IGNORE:
		default:
			return;
	}
	
	switch(get_class()->stack_policy) {
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

    stack += get_class()->stack_delta;
    if (stack > get_class()->max_stack) {
        stack = get_class()->max_stack;
    }
    if (stack < 0) {
        return false;
    }
    
    strength += get_class()->strength_delta;
    if (strength < 0) {
        return false;
    }
    
    return true;
}

// STATS =========================

bool condition::has_stat(stats_t code){

	return get_class()->stats->has_stat(code);
}

int condition::get_stat(stats_t code){

	int value = 0;

	if (has_stat(code)){
	
		value = get_class()->stats->get_stat(code);
	}
	
	return value;
}

int condition::get_modified_stat(stats_t code) {
	return get_stat(code) * (stack * get_class()->stack_stats);
}

int condition::get_modified_strength() {
	return strength * (stack * get_class()->stack_strength);
}

// EFFECTS ============================================

void condition::resolve_trigger(trigger_t trigger, argmap * args) {
	
    argmap * my_map = new argmap();
    my_map->add_condition(ARG_HOLDER_CONDITION, this);
    
	if (args != NULL) {
		args->add_args(my_map);
	}
    
    // Execute all these effects
    vector<effect*> * my_effects = get_class()->get_effects(trigger);
    if (my_effects != NULL) {
        for(int i = 0; i < my_effects->size(); ++i) {
            do_effect(my_map, my_effects->at(i));
        }
    }
}
