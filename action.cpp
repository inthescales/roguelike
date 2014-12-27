#include "action.h"

// Actionblock stuff ===================

targetActionBlock::targetActionBlock(string nprompt, argmap * nargs, target_t ntarg, radius_t nrad, actionRole_t npos) {

    args = nargs;
    requirements = new vector<requirement*>();
    target_type = ntarg;
    radius_type = nrad;
    position = npos;
    prompt = nprompt;
    block_type = TARGET_BLOCK;
}

effectActionBlock::effectActionBlock(argmap * nargs, effect * neff) {

    args = nargs;
    requirements = new vector<requirement*>();
    eff = neff;
    block_type = EFFECT_BLOCK;
}

requirementActionBlock::requirementActionBlock(bool nloop, bool nend, argmap * nargs) {

    args = nargs;
    requirements = new vector<requirement*>();
    isLoop = nloop;
    endBlock = nend;
    block_type = REQUIREMENT_BLOCK;
}

// Action itself =====================

action::action(argmap * nargs) {
    args = nargs;
    blocks = new vector<actionBlock*>();
}

action::action(argmap * nargs, vector<actionBlock *> * nblocks) {

    args = nargs;
    blocks = nblocks;
}

void action::add_block(actionBlock * nblock) {

    blocks->push_back(nblock);
}

// Requirement blocks ===============

bool requirementActionBlock::evaluate() {

    vector<requirement*>::iterator it = requirements->begin();
    
    // Pass role arguments onto requirements from block
    for(; it != requirements->end(); ++it) {
    
        if(args->has_value(ARG_ACTION_AGENT)) {
            (*it)->args->add_actor(ARG_ACTION_AGENT, args->get_actor(ARG_ACTION_AGENT));
        }
        if(args->has_value(ARG_ACTION_PATIENT)) {
            (*it)->args->add_vector(ARG_ACTION_PATIENT, args->get_vector(ARG_ACTION_PATIENT));
        }
        if(args->has_value(ARG_ACTION_INSTRUMENT)) {
            (*it)->args->add_vector(ARG_ACTION_INSTRUMENT, args->get_vector(ARG_ACTION_INSTRUMENT));
        }
    }

    if (args->has_value(ARG_ACTION_AGENT)) {
        return requirement::check_requirements_for((mapentity *)args->get_actor(ARG_ACTION_AGENT), requirements);
    } else {
        return requirement::check_requirements(requirements);
    }
}
