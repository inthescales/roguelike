#include "condclass.h"
#include "effect.h"

condclass::condclass(string nname, colorName ncolor, int nstrength, int nduration, int ninterval, int nvalue,
                     cond_stack_policy_t nstackpol, cond_timer_policy_t ntimerpol) {

    name = nname;
    color = ncolor;
    base_strength = nstrength;
    base_duration = nduration;
    decay_interval = ninterval;
    decay_value = nvalue;
    stack_policy = nstackpol;
    timer_policy = ntimerpol;
	stats = new statmap();
}

void condclass::add_effect(trigger_effect n){

	effects.push_back(n);
}