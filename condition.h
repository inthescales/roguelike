#ifndef CONDITION_H

#define CONDITION_H

#include "argmap.h"
#include "effect.h"
#include "timer.h"
#include <vector>

using std::vector;

class condition {

	public:
	
	int type;
	int stack;
    int strength;
    int duration;
    
    vector<timer *> * timer_list;
	
	//Functions:
	condition(int);
	condition(int, int);
	condition(int, int, int);
	
    void destroy();
    
    void start_timers();
    void destroy_timers();
    
	void add_condition(condition *);
	bool do_decay();
	
	bool has_stat(stats_t);
	int get_stat(stats_t);
	int get_modified_stat(stats_t);
	int get_modified_strength();
	
	effect * get_effect(trigger_t);
	void resolve_trigger(trigger_t, argmap *);
};

#endif

/*

    Design for condition stacking and decay -
    
    Stack records number of stacked identical conditions. Stacking will multiply bonuses to stats, and possibly
    be read by related effects. Strength determines the power of effects linked to this condition outside of stats.
    Strength and stack may interact, depending on the condition.
    
    For conditions that change in strength over time, we need deltas for stack, strength, and time. The time delta
    is a feature of the timer and will be stored there. We'll have strength and stack deltas separately. For stacking
    effect conditions, stack delta will be -1. For decaying / growing conditions, these may be positive and an iteration
    limit will be used to end the condition. This will also be carried in the timer, in the iter field.
    
    For conditions that transition into other types of conditions, a condition_timeout effect will be used
    
    Essentially there are three condition cases:
    
    Stacking condition:
    This condition has a bonus or malus to stats. It can be stacked multiple times, multiplying that stat modifier. It has a
    max_stack greater than 1. It might have a strength for a non-stat related effect. This strength may or may not be modified
    by the stack - this takes place in the effect. The condition creates a timer to regulate the decrease of stack over time.
    The timer will have a COND_STACK_DECAY effect (or w/e) which iterates endlessly, and the condition cleans itself and its timer
    up when the stack runs out. Alternately, it could increase stack over time and have an iteration limit or time delta condition
    for ending the condition. Alternately, it could never end and simply persist until it's removed.
    
    Decaying condition:
    This condition does not stack, and so have a max_stack of 1, though its stack policy might be used to determine whether to roll back
    decay. The condition class has a strength delta that alters the strength over time - the strength in the condition begins at the cond.
    class' base_strength, but is decreased automatically by the timer whenever it resets according to the strength_delta.
    Note - apparently the only difference between this case and the stacking condition case with decreasing stacks over time is that stacks
    are presumed to automatically alter the magnitude of stat modifiers and NOT strength, while the decaying condition case decreases strength
    only, leaving stacks and therefore stat modifiers, unchanged.
    
    Question - is it possible to decay both stacks and strength?
    Answer - Condition class with strength_delta and stack_delta, and a max_stack greater than 1. Ok that was easy.
    
    Chaining condition:
    This is just multiple conditions with ending effects that add a new condition in the chain. Remember to have multiple effect triggers for
    different circumstances of condition end - TRIGGER_COND_END, TRIGGER_COND_TIMEOUT, TRIGGER_COND_REMOVED
    
    Side note:
    Timing for conditions is set when the condition is created. It's added directly into the timer, and so can be controlled by
    whatever source is creating the condition. The condition class has an iterations field and a duration and duration delta, so
    it could set up the timer on its own. This -is- good since in many condition cases I won't want to track those things specifically.
    Nevertheless, it's good to remember that I can have different sources that apply the same condition with different timing properties.
    Likewise, I could ignore the condition class' base_strength and give a condition whatever strength I want, though custom stats are a bit
    tricky. It's probably best to avoid ever doing that though as behavior when two conditions of the same type but with different strength/
    stats come into contact is undefined. I could do something with stack policy, but it's probably best to avoid this situation entirely.

*/