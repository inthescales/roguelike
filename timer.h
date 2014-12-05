#ifndef TIMER_H

#define TIMER_H

class argmap;
class effect;
class condition;

class timer {

    public:
    effect * executed_effect;
    argmap * effect_args;

    int time;      // Time after the previous event that this event will occur.
    int base_time; // Original time. Needed for renewing the timer
    int iters;     // Iterations to occur before ending
    int delta;     // change in time between iterations

    timer(effect *, argmap *, int, int, int);
    timer * create_child();
    
    bool execute();
};

#endif

/*

    Timer deletion and modification:
    Cases we need to handle:
    - An entity, such as an actor with a timed condition on its type, or a condition with a timed effect,
    is destroyed. We should destroy its timer to end the effect.
    
    - An entity with a condition receives that condition again, or some other case where we need to modify
    the timing of an existing timer.
        - time policy cases:
            - SET / ADD: change timer's time, check subsequent timers in case we need to move it back
            - MAX / MIN: do that, or do nothing depending on values.
    
    To delete, we could just set a delete tag and ignore and delete the timer when it next comes around.
    Wait a minute... couldn't I just put a note on the timer to change time the next time it happens?
    Oh, but then I can only increase the time but not decrease it.
    
    Ok I'll probably have to go with the long solution, at least for setting time. It's no worse than adding
    a timer.
    
    So the solution will be:
    Entities will manage references to timers they created. When the entity is destroyed, they can mark the
    delete flag on their timers. To change the time, we'll have to iterate through the list of timers and 
    find the one we're changing, then set its time appropriately and move it if needed.
    
    Moving is a little tricky. The best solution is probably to remove the timer, bridge the area it's leaving
    by changing the following timer's value, then readd it.
*/