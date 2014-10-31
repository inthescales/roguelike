#include "timer.h"

#include "argmap.h"
#include "effect.h"

timer::timer(effect * my_effect, argmap * my_args, int my_time, int my_iter, int my_delta) {
    executed_effect = my_effect;
    effect_args = my_args;
    base_time = time = my_time; // Set time and base time. Time will be modified when adding to event queue.
    iters = my_iter;
    delta = my_delta;
}

// Execute the effect contained in this timer
void timer::execute() {

    // Can update args against saved entity if necessary. Entity will probably tend to be the argument anyway though.
    do_effect(effect_args, executed_effect);
}

// Create a new instance of this timer modifying the iterations and time as necessary
timer * timer::create_child() {
    if (iters == 0 || base_time - delta <= 0) {
        return NULL;
    }
    
    timer * r = new timer(executed_effect, effect_args, base_time - delta, iters - 1, delta);
    return r;
}