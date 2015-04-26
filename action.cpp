#include "action.h"
#include "classdefs.h"

action * actiondef[ACTION_MAX];

// Actionblock stuff ===================

targetActionBlock::targetActionBlock(string nprompt, target_t ntarg, radius_t nrad, extract_t next,  actionRole_t npos) {

    args = new argmap();
    requirements = new vector<requirement*>();
    target_type = ntarg;
    radius_type = nrad;
    extract_type = next;
    position = npos;
    prompt = nprompt;
    block_type = TARGET_BLOCK;
    testDone = false;
}

targetActionBlock::targetActionBlock(string nprompt, target_t ntarg, radius_t nrad, extract_t next, actionRole_t npos, argmap * nargs) {

    args = nargs;
    requirements = new vector<requirement*>();
    target_type = ntarg;
    radius_type = nrad;
    extract_type = next;
    position = npos;
    prompt = nprompt;
    block_type = TARGET_BLOCK;
    testDone = false;
}

extractActionBlock::extractActionBlock(extract_t etype, actionRole_t frompos, actionRole_t topos) {

    args = new argmap();
    requirements = new vector<requirement*>();
    extract_type = etype;
    from_position = frompos;
    to_position = topos;
    block_type = EXTRACT_BLOCK;
    testDone = false;
}

effectActionBlock::effectActionBlock(effect * neff) {

    args = new argmap();
    requirements = new vector<requirement*>();
    eff = neff;
    block_type = EFFECT_BLOCK;
    testDone = false;
}

effectActionBlock::effectActionBlock(argmap * nargs, effect * neff) {

    args = nargs;
    requirements = new vector<requirement*>();
    eff = neff;
    block_type = EFFECT_BLOCK;
    testDone = false;
}

requirementActionBlock::requirementActionBlock(bool nloop, bool nend, bool ncrit) {

    args = new argmap();
    requirements = new vector<requirement*>();
    isLoop = nloop;
    endBlock = nend;
    critical = ncrit;
    block_type = REQUIREMENT_BLOCK;
    testDone = false;
}

requirementActionBlock::requirementActionBlock(bool nloop, bool nend, bool ncrit, argmap * nargs) {

    args = nargs;
    requirements = new vector<requirement*>();
    isLoop = nloop;
    endBlock = nend;
    critical = ncrit;
    block_type = REQUIREMENT_BLOCK;
    testDone = false;
}

// Action itself =====================

action::action() {
    args = new argmap();
    blocks = new vector<actionBlock*>();
    priority = 0;
    contextOk = false;
    equipOnly = false;
}

action::action(argmap * nargs) {
    args = nargs;
    blocks = new vector<actionBlock*>();
    priority = 0;
    contextOk = false;
    equipOnly = false;
}

action::action(argmap * nargs, vector<actionBlock *> * nblocks) {

    args = nargs;
    blocks = nblocks;
    priority = 0;
    contextOk = false;
    equipOnly = false;
}

void action::add_block(actionBlock * nblock) {

    blocks->push_back(nblock);
}

// Requirement block management ===============

void actionBlock::add_requirement(requirement * nreq) {

    requirements->push_back(nreq);
}

set<error_t> * requirementActionBlock::evaluate() {

    vector<requirement*>::iterator it = requirements->begin();

    return requirement::check_requirements(requirements, args);
}

// Helper functions ============================

// Compare priority of two actions
bool priority_comp(int a, int b) {

    return actiondef[a]->priority >= actiondef[b]->priority;
}

vector<action*> * action::defs_for(vector<int> * in) {

    vector<action*> * ret = new vector<action*>();
    vector<int>::iterator it = in->begin();
    for(;it != in->end(); ++it) {
        ret->push_back(actiondef[*it]);
    }
    
    return ret;
}

args_t actrole_to_arg(actionRole_t role) {
    
    switch (role) {
    
        case ACTROLE_AGENT:
        return ARG_ACTION_AGENT;
        
        case ACTROLE_PATIENT:
        return ARG_ACTION_PATIENT;
        
        case ACTROLE_INSTRUMENT:
        return ARG_ACTION_INSTRUMENT;
        
        case ACTROLE_LOCATION:
        return ARG_ACTION_LOCATION;
    
        default:
        break;
    }
    
    return ARG_NONE;    
}

// TODO - in requirements, add else blocks, and target extract blocks (get feature from last tile, etc)
