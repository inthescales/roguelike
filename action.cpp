#include "action.h"
#include "actor.h"
#include "classdefs.h"
#include "effect.h"
#include "interface.h"
#include "pathing.h"
#include "rogue.h"

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

// Functions for executing and testing actions ======================


set<error_t> * action::execute() {
    
    return execute(NULL, true, false);
}

set<error_t> * action::execute(argmap * args, bool get_targets) {
    return execute(args, get_targets, false);
}

// Execute an action, including selecting targets. Returns set of errors. If NULL,
// action completed successfully
set<error_t> * action::execute(argmap * args, bool get_targets, bool test) {

    set<error_t> * errs = new set<error_t>();
    
    if (blocks == NULL) {
        errs->insert(ERR_BAD_INPUT);
        return errs;
    }

    actionBlock * curBlock;
    bool failedReq = false;

    // implement stack for loops
    argmap * roles = new argmap(); // NOT using normal arg keys. Using action role keys instead.
    if (args) {
        if (args->has_value(ARG_ACTION_AGENT)) roles->add_actor((args_t)ACTROLE_AGENT, args->get_actor(ARG_ACTION_AGENT));
        if (args->has_value(ARG_ACTION_PATIENT)) roles->add_vector((args_t)ACTROLE_PATIENT, args->get_vector(ARG_ACTION_PATIENT));
        if (args->has_value(ARG_ACTION_INSTRUMENT)) roles->add_vector((args_t)ACTROLE_INSTRUMENT, args->get_vector(ARG_ACTION_INSTRUMENT));
        if (args->has_value(ARG_ACTION_LOCATION)) roles->add_vector((args_t)ACTROLE_LOCATION, args->get_vector(ARG_ACTION_LOCATION));
    }
    
    // Process each block in the action in order
    for (int i = 0; i < blocks->size(); ++i) {
    
        curBlock = blocks->at(i);
        
        // If we failed the last requirement block and haven't seen an end yet, skip instruction.
        if (failedReq && !(curBlock->block_type == REQUIREMENT_BLOCK && ((requirementActionBlock *)curBlock)->endBlock)) continue;
        
        if (curBlock->block_type != TARGET_BLOCK) {
            if(roles->has_value((args_t)ACTROLE_AGENT)) {
                curBlock->args->add_actor(ARG_ACTION_AGENT, roles->get_actor((args_t)ACTROLE_AGENT));
            }
            if(roles->has_value((args_t)ACTROLE_PATIENT)) {
                curBlock->args->add_vector(ARG_ACTION_PATIENT, roles->get_vector((args_t)ACTROLE_PATIENT));
            }
            if(roles->has_value((args_t)ACTROLE_INSTRUMENT)) {
                curBlock->args->add_vector(ARG_ACTION_INSTRUMENT, roles->get_vector((args_t)ACTROLE_INSTRUMENT));
            }
            if(roles->has_value((args_t)ACTROLE_LOCATION)) {
                curBlock->args->add_vector(ARG_ACTION_LOCATION, roles->get_vector((args_t)ACTROLE_LOCATION));
            }
            
        }
        
        switch (curBlock->block_type) {
        
            // TARGET BLOCK - get vector returned by target function, set semantic role
            case TARGET_BLOCK:
            {
                if (get_targets) {
                    actor * agent = roles->get_actor((args_t)ACTROLE_AGENT);
                    vector<void*> * targets = agent->select_target((targetActionBlock *)curBlock);
                    switch(((targetActionBlock *)curBlock)->position) {
                    
                        case ACTROLE_AGENT:
                            roles->add_actor((args_t)ACTROLE_AGENT, (actor*)targets->at(0));
                        break;
                        case ACTROLE_PATIENT:
                            roles->add_vector((args_t)ACTROLE_PATIENT, targets);
                        break;
                        case ACTROLE_INSTRUMENT:
                            roles->add_vector((args_t)ACTROLE_INSTRUMENT, targets);
                        break;
                        case ACTROLE_LOCATION:
                            roles->add_vector((args_t)ACTROLE_LOCATION, targets);
                        break;
                    }
                } else if(test) {
                
                    // Make sure targets fit the requirements and target arguments
                    entity * agent = (entity*)roles->get_actor((args_t)ACTROLE_AGENT);
                    vector<void*> * targs = (vector<void*> *)args->get_vector(actrole_to_arg(((targetActionBlock *)curBlock)->position));
                    vector<void*>::iterator it = targs->begin();
                    set<error_t> * targerrs = new set<error_t>();
                                        
                    for(; it != targs->end(); ++it) {
                        set<error_t> * newerrs = verify_target((targetActionBlock *)curBlock, agent, (entity*)(*it));
                        if (newerrs != NULL) {
                            targerrs->insert(newerrs->begin(), newerrs->end());
                        }
                    }
                    
                    if (targerrs->size() > 0) {
                        return targerrs;
                    }
                }
            }
            break;
            
            // EXTRACT BLOCK - take role, run extraction function, store in new role
            case EXTRACT_BLOCK:
            {
                vector<void*> * extracted;
                switch (((extractActionBlock *)curBlock)->from_position) {
                    case ACTROLE_AGENT:
                        extracted = new vector<void*>();
                        extracted->push_back((void*)roles->get_actor((args_t)ACTROLE_AGENT));
                    break;
                    default:
                        extracted = roles->get_vector((args_t)((extractActionBlock *)curBlock)->from_position);
                    break;
                }
            
                switch (((extractActionBlock *)curBlock)->extract_type) {
                    case EXT_COPY:
                        // Do nothing! Write to the destination as it is
                    break;
                    case EXT_ACTOR:
                        extracted = (vector<void*>*)UI::extract_actors((vector<tile*> *)extracted, curBlock->requirements);
                    break;
                    case EXT_OBJECTS:
                        extracted = (vector<void*>*)UI::extract_objects((vector<tile*> *)extracted, curBlock->requirements);
                    break;
                    case EXT_INVENTORY:
                        extracted = (vector<void*>*)UI::extract_inventories((vector<tile*> *)extracted, curBlock->requirements);
                    break;
                    case EXT_FEATURE:
                        extracted = (vector<void*>*)UI::extract_features((vector<tile*> *)extracted, curBlock->requirements);
                    break;
                }
                
                switch(((extractActionBlock *)curBlock)->to_position) {
                
                    case ACTROLE_AGENT:
                        roles->add_actor((args_t)ACTROLE_AGENT, (actor*)extracted->at(0));
                    break;
                    default:
                        roles->add_vector((args_t)((extractActionBlock *)curBlock)->to_position, extracted);
                    break;
                }
            }
            break;
            
            // EFFECT BLOCK - set roles, process effect
            case EFFECT_BLOCK:
                if (test) { // If we're testing, return if this block says to
                    if (curBlock->testDone) {
                        return NULL;
                    }
                } else { // Only execute effects if we aren't testing.
                     ((effectActionBlock *)curBlock)->eff->resolve(curBlock->args);
                }
            break;
                
            // REQUIREMENT BLOCK - set roles, check for requirements
            case REQUIREMENT_BLOCK:
            {
                // TODO - Implement loops
                std::set<error_t> * errset = ((requirementActionBlock *)curBlock)->evaluate();
                if (errset != NULL && ((requirementActionBlock *)curBlock)->critical) {
                    // Failed requirement means action cannot count as completed
                    return errset;
                }
            }
            break;
          
        }
        
    }
    
    return NULL;
}

set<error_t> * action::test(argmap * args) {
    return execute(args, false, true);
}

/*
    Check whether an action target block could be used to target the specified
    entity. Return a set of errors if any, NULL otherwise.
*/
set<error_t> * action::verify_target(targetActionBlock * block, entity * agent, entity * target) {

    set<error_t> * errs = new set<error_t>();
    
    // Check the block's targeting args
    if (block->args->has_value(ARG_TARGET_ENTITY_TYPE)) {

        if (agent == target && block->args->has_flag(FLAG_TARGET_NOT_SELF)) {
            errs->insert(ERR_CANT_TARGET_SELF);
        } else
        if (agent != target && block->args->has_flag(FLAG_TARGET_SELF_ONLY)) {
            errs->insert(ERR_MUST_TARGET_SELF);
        }
    }

    if (block->args->has_value(ARG_TARGET_MIN_DISTANCE)) {
        int dist = pathing::distance_between((mapentity*)agent, (mapentity*)target);
        if (dist < block->args->get_int(ARG_TARGET_MIN_DISTANCE)) {
            errs->insert(ERR_TOO_CLOSE);
        }
    }
    
    if (block->args->has_value(ARG_TARGET_MAX_DISTANCE)) {
        int dist = pathing::distance_between((mapentity*)agent, (mapentity*)target);
        if (dist > block->args->get_int(ARG_TARGET_MAX_DISTANCE)) {
            errs->insert(ERR_TOO_FAR);
        }
    }
    
    // Check the block's requirements
    vector<requirement*>::iterator it = block->requirements->begin();
    for(; it != block->requirements->end(); ++it) {
        set<error_t> * reqerrs = (*it)->check_for(target);
        errs->insert(reqerrs->begin(), reqerrs->end());
    }
    
    if (errs->size() > 0) {
        return errs;
    } else {
        return NULL;
    }    
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
