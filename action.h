#ifndef ACTION_H
#define ACTION_H

#include "argmap.h"
#include <vector>

#include "enums.h"

using std::vector;

enum action_block_t {
    TARGET_BLOCK = 0,
    EFFECT_BLOCK
};

class actionBlock {

    public:
    action_block_t block_type;
    argmap * args;
};

class targetActionBlock : actionBlock {
    
    public:
    target_t target_type;
    radius_t radius_type;
    args_t position;
    
    targetActionBlock(argmap *, target_t, radius_t, args_t);
};

class effectActionBlock : actionBlock {

    public:
    effect_t effect_type;
    
    effectActionBlock(argmap *, effect_t);
};

class action {

    argmap * requirements;
    vector<actionBlock *> * blocks;
    
    action(argmap *, vector<actionBlock *> *);
    void execute();
};

#endif

/*

Need a series of steps for determining targets and instruments
Really, we could have a whole set of these so it needs to be an
arbitrarily long, multi-step process.

I'd like to be passing a list of things into the actual do_effect

OK!
Action contains vector of pairs. The first elements is a targeting
type, which includes both how the thing is picked and what type it
is. The second element is an argmap code for where the target will
be stored. These can be read by the effect.


An action needs to be able to have multiple effects.

Idea:
vector<tuple<target, radius, vector<effect> > >

All of these are executed.


*/