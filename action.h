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

enum actionPurpose_t {
    ACTPUR_NONE = 0,
    ACTPUR_MOVE,
    ACTPUR_TAKE,
    ACTPUR_HARM,
    ACTPUR_EAT,
    ACTPUR_DRINK,
    ACTPUR_OPEN_FEAT,
    ACTPUR_CLOSE_FEAT
};

class effect;
class requirement;

class actionBlock {

    public:
    action_block_t block_type;
    
    vector<requirement*> * requirements; // Act a bit differently for block types
    argmap * args;
    
    void add_requirement(requirement *);
};

// Only targets fitting requirements will be selected
class targetActionBlock : public actionBlock {
    
    public:
    target_t target_type;
    radius_t radius_type;
    actionRole_t position;
    string prompt;
    
    targetActionBlock(string, target_t, radius_t, actionRole_t);
    targetActionBlock(string, target_t, radius_t, actionRole_t, argmap *);
};

// Continue affecting targets as long as the condition is true
class effectActionBlock : public actionBlock {

    public:
    effect * eff;
    
    effectActionBlock(effect *);
    effectActionBlock(argmap *, effect *);
};

// Subsequent blocks will only execute if requirements are met
class requirementActionBlock : public actionBlock {
    
    public:
    bool isLoop;
    bool endBlock;
    bool critical;
    
    requirementActionBlock(bool, bool, bool);
    requirementActionBlock(bool, bool, bool, argmap *);
    bool evaluate();
};

class action {

    public:
    actionPurpose_t purpose;
    int priority;
    bool contextOk;
    bool equipOnly;
    
    argmap * args;
    vector<actionBlock *> * blocks;
    
    action();
    action(argmap *);
    action(argmap *, vector<actionBlock *> *);
    void add_block(actionBlock *);
};

bool priority_comp(int, int);

#endif
