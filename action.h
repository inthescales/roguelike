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
    EXTRACT_BLOCK,
    EFFECT_BLOCK,
    REQUIREMENT_BLOCK
};

enum target_t {
    TAR_NONE = 0,
	TAR_SELF, // Target self
	TAR_ADJ, // Target an adjacent tile
    TAR_TILE, // Target a specific tile
};

enum radius_t {
    RAD_NONE = 0,
	RAD_SINGLE,
    RAD_LINE
};

enum extract_t {
    EXT_TILE = 0,
    EXT_ACTOR,
    EXT_FEATURE,
    EXT_OBJECTS,
    EXT_INVENTORY
};

enum actionRole_t {
    ACTROLE_AGENT,
    ACTROLE_PATIENT,
    ACTROLE_PATIENT_2,
    ACTROLE_INSTRUMENT,
    ACTROLE_LOCATION,
    ACTROLE_MAX
};

enum actionPurpose_t {
    ACTPUR_NONE = 0,
    ACTPUR_MOVE,
    ACTPUR_TAKE,
    ACTPUR_DROP,
    ACTPUR_EQUIP,
    ACTPUR_UNEQUIP,
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
    extract_t extract_type;
    actionRole_t position;
    string prompt;
    
    targetActionBlock(string, target_t, radius_t, extract_t, actionRole_t);
    targetActionBlock(string, target_t, radius_t, extract_t, actionRole_t, argmap *);
};

// Allow
class extractActionBlock : public actionBlock {

    public:
    extract_t extract_type;
    actionRole_t from_position;
    actionRole_t to_position;
    
    extractActionBlock(extract_t, actionRole_t, actionRole_t);
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

extern action * actiondef[];

bool priority_comp(int, int);

#endif
