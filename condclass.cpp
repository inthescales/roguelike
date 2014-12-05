#include "condclass.h"
#include "effect.h"

condclass::condclass(string nname, colorName ncolor, int nstrength, int nduration, int stack_d, int strength_d, int duration_d, int n_iters,
                     bool s_stat, bool s_str, int n_maxstack, cond_stack_policy_t nstackpol, cond_timer_policy_t ntimerpol) {

    name = nname;
    color = ncolor;
    
    base_strength = nstrength;
    base_duration = nduration;
    
    stack_delta = stack_d;
    strength_delta = strength_d;
    duration_delta = duration_d;
    iterations = n_iters;
    
    stack_stats = s_stat;
    stack_strength = s_str;
    
    max_stack = n_maxstack;
    stack_policy = nstackpol;
    timer_policy = ntimerpol;
    
	stats = new statmap();
}

void condclass::add_effect(trigger_effect n){

	effects.push_back(n);
}