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
    void execute();
};

#endif
