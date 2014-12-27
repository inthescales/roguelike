#ifndef ACTION_H
#define ACTION_H

#include "argmap.h"
#include "requirement.h"
#include <string>
#include <vector>

#include "enums.h"

using std::string;
using std::vector;

enum action_block_t {
    TARGET_BLOCK = 0,
    EFFECT_BLOCK,
    REQUIREMENT_BLOCK
};

enum target_t {
    TAR_NONE = 0,
	TAR_SELF, // Target self
	TAR_ADJ, // Target an adjacent entity
    TAR_INV, // Target an object or objects from inventory
};

enum actionRole_t {
    ACTROLE_AGENT,
    ACTROLE_PATIENT,
    ACTROLE_INSTRUMENT,
    ACTROLE_MAX
};

class effect;
class requirement;

class actionBlock {

    public:
    action_block_t block_type;
    
    vector<requirement*> * requirements; // Act a bit differently for block types
    argmap * args;
};

// Only targets fitting requirements will be selected
class targetActionBlock : public actionBlock {
    
    public:
    target_t target_type;
    radius_t radius_type;
    actionRole_t position;
    string prompt;
    
    targetActionBlock(string, argmap *, target_t, radius_t, actionRole_t);
};

// Continue affecting targets as long as the condition is true
class effectActionBlock : public actionBlock {

    public:
    effect * eff;
    
    effectActionBlock(argmap *, effect *);
};

// Subsequent blocks will only execute if requirements are met
class requirementActionBlock : public actionBlock {
    
    public:
    bool isLoop;
    bool endBlock;
    
    requirementActionBlock(bool nloop, bool nend, argmap *);
    bool evaluate();
};

class action {

    public:
    argmap * args;
    vector<actionBlock *> * blocks;
    
    action(argmap *, vector<actionBlock *> *);
    action(argmap *);
    void add_block(actionBlock *);
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