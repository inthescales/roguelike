#include "timer.h"

#include "argmap.h"
#include "condition.h"
#include "effect.h"

timer::timer(timer_effect * my_effect, argmap * my_args) {

    executed_effect = my_effect;
    effect_args = my_args;
    base_time = time = my_effect->time;
    iters = my_effect->iterations;
    delta = my_effect->delta;
}

timer::timer(effect * my_effect, argmap * my_args, int my_time, int my_iter, int my_delta) {
    executed_effect = my_effect;
    effect_args = my_args;
    base_time = time = my_time; // Set time and base time. Time will be modified when adding to event queue.
    iters = my_iter;
    delta = my_delta;
}

// Execute the effect contained in this timer
// Returns true if nothing is preventing the timer from being reset, false otherwise
bool timer::execute() {

    bool can_continue = true;
    
    executed_effect->resolve(effect_args);
      
    // Process decay on any linked conditions. Check whetehr the effect is able to continue.
    if (effect_args->get_condition(ARG_HOLDER_CONDITION) != NULL) {
        condition * linked_cond = effect_args->get_condition(ARG_HOLDER_CONDITION);
        can_continue = linked_cond->do_decay();
    }
    
    return can_continue;
}

// Create a new instance of this timer modifying the iterations and time as necessary
timer * timer::create_child() {
    if (iters == 0 || base_time - delta <= 0) {
        return NULL;
    }
    
    timer * r = new timer(executed_effect, effect_args, base_time - delta, iters - 1, delta);
    return r;
}