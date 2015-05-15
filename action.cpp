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

extractActionBlock::extractActionBlock(extract_t etype, args_t frompos, args_t topos) {

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

// Action response ===================

action_resp::action_resp() {
    
    status = ACTRES_UNKNOWN;
    successes = new vector<void*>();
    errors = new vector<error*>();
}

/*
    Merge two require responses.
    Result has intersection of successes, union of errors.
    State is set accordingly.
*/
void action_resp::merge(action_resp * other) {

    // Keep my successes only if the other agrees
    vector<void*> * new_succs = new vector<void*>();
    vector<void*>::iterator it = successes->begin();
    vector<void*>::iterator it2 = other->successes->begin();
    for(; it != successes->end(); ++it) {
        for(; it2 != other->successes->end(); ++it2) {
        
            if((*it) == (*it2)) {
                new_succs->push_back(*it);
                break;
            }
        }
    }

    // Add other's errors to my own - they won't overlap
    vector<error*>::iterator err_it = other->errors->begin();
    for(; err_it != other->errors->end(); ++err_it) {
        errors->push_back(*err_it);
    }
    
    successes = new_succs;
    
    if (errors->size() > 0) {
        if (successes->size() > 0) {
            status = ACTRES_PARTIAL;
        } else {
            status = ACTRES_FAILURE;
        }        
    } else {
        status = ACTRES_SUCCESS;
    }
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

require_resp * requirementActionBlock::evaluate(argmap * instanceArgs) {

    vector<requirement*>::iterator it = requirements->begin();

    return requirement::evaluate_multiple(requirements, instanceArgs);
}

// Functions for executing and testing actions ======================


action_resp * action::execute() {
    
    return execute(NULL, true, false);
}

action_resp * action::execute(argmap * args, bool get_targets) {
    return execute(args, get_targets, false);
}

// Execute an action, including selecting targets. Returns set of errors. If NULL,
// action completed successfully
action_resp * action::execute(argmap * args, bool get_targets, bool test) {

    action_resp * response = new action_resp();
    
    if (blocks == NULL) {
        response->errors->push_back(new error(ERR_BAD_INPUT));
        return response;
    }

    actionBlock * curBlock;
    bool failedReq = false;

    // implement stack for loops
    argmap * instanceArgs = new argmap();
    args->add_args(instanceArgs, true);
    
    // Process each block in the action in order
    for (int i = 0; i < blocks->size(); ++i) {
    
        curBlock = blocks->at(i);
        
        // If we failed the last requirement block and haven't seen an end yet, skip instruction.
        if (failedReq && !(curBlock->block_type == REQUIREMENT_BLOCK && ((requirementActionBlock *)curBlock)->endBlock)) continue;
        
        switch (curBlock->block_type) {
        
            // TARGET BLOCK - get vector returned by target function, set semantic role
            case TARGET_BLOCK:
            {
                if (get_targets) {
                    actor * agent = (actor *)instanceArgs->get_from_vector(ARG_ACTION_AGENT);
                    vector<void*> * targets = agent->select_target((targetActionBlock *)curBlock);
                    switch(((targetActionBlock *)curBlock)->position) {
                    
                        case ACTROLE_AGENT:
                            instanceArgs->add_vector((args_t)ARG_ACTION_AGENT, targets);
                        break;
                        case ACTROLE_PATIENT:
                            instanceArgs->add_vector((args_t)ARG_ACTION_PATIENT, targets);
                        break;
                        case ACTROLE_INSTRUMENT:
                            instanceArgs->add_vector((args_t)ARG_ACTION_INSTRUMENT, targets);
                        break;
                        case ACTROLE_LOCATION:
                            instanceArgs->add_vector((args_t)ARG_ACTION_LOCATION, targets);
                        break;
                    }
                } else if(test) {
                
                    // Make sure targets fit the requirements and target arguments
                    entity * agent = (entity*)instanceArgs->get_from_vector((args_t)ARG_ACTION_AGENT);
                    vector<void*> * targs = (vector<void*> *)args->get_vector(actrole_to_arg(((targetActionBlock *)curBlock)->position));
                    vector<void*>::iterator it = targs->begin();
                    vector<error*> * targerrs = new vector<error*>();
                                        
                    for(; it != targs->end(); ++it) {
                        vector<error*> * newerrs = verify_target((targetActionBlock *)curBlock, agent, (entity*)(*it));
                        if (newerrs != NULL) {
                            targerrs->insert(targerrs->end(), newerrs->begin(), newerrs->end());
                        }
                    }
                    
                    if (targerrs->size() > 0) {
                        response->errors->insert(response->errors->end(), targerrs->begin(), targerrs->end());
                        return response;
                    }
                }
            }
            break;
            
            // EXTRACT BLOCK - take role, run extraction function, store in new role
            case EXTRACT_BLOCK:
            {
                vector<void*> * extracted;
                extracted = instanceArgs->get_vector((args_t)((extractActionBlock *)curBlock)->from_position);

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

                instanceArgs->add_vector((args_t)((extractActionBlock *)curBlock)->to_position, extracted);
            }
            break;
            
            // REQUIREMENT BLOCK - set args, check for requirements
            case REQUIREMENT_BLOCK:
            {
                // TODO - Implement loops
                require_resp * reqresp = ((requirementActionBlock *)curBlock)->evaluate(instanceArgs);
                
                // If this requirement is critical, abort the action
                if (reqresp->result != REQRES_SUCCESS && ((requirementActionBlock *)curBlock)->critical) {

                    response->errors->insert(response->errors->end(), reqresp->errors->begin(), reqresp->errors->end());
                    return response;
                }
            }
            break;
            
            // EFFECT BLOCK - set args, process effect
            case EFFECT_BLOCK:
            {
                if (curBlock->testDone) {
                    response->status = ACTRES_SUCCESS;
                    if (test) { // If we're testing, return if this block says to
                        return response;
                    }
                }

                effect_resp * eff_resp = ((effectActionBlock *)curBlock)->eff->resolve(instanceArgs);
            }
            break;
          
        }
        
    }
    
    return response;
}

action_resp * action::test(argmap * args) {
    return execute(args, false, true);
}

/*
    Check whether an action target block could be used to target the specified
    entity. Return a set of errors if any, NULL otherwise.
*/
vector<error*> * action::verify_target(targetActionBlock * block, entity * agent, entity * target) {

    vector<error*> * errs = new vector<error*>();
    
    // Check the block's targeting args
    if (block->args->has_value(ARG_TARGET_ENTITY_TYPE)) {

        if (agent == target && block->args->has_flag(FLAG_TARGET_NOT_SELF)) {
            errs->push_back(new error(ERR_CANT_TARGET_SELF));
        } else
        if (agent != target && block->args->has_flag(FLAG_TARGET_SELF_ONLY)) {
            errs->push_back(new error(ERR_MUST_TARGET_SELF));
        }
    }

    if (block->args->has_value(ARG_TARGET_MIN_DISTANCE)) {
        int dist = pathing::distance_between((mapentity*)agent, (mapentity*)target);
        if (dist < block->args->get_int(ARG_TARGET_MIN_DISTANCE)) {
            errs->push_back(new error(ERR_TOO_CLOSE));
        }
    }
    
    if (block->args->has_value(ARG_TARGET_MAX_DISTANCE)) {
        int dist = pathing::distance_between((mapentity*)agent, (mapentity*)target);
        if (dist > block->args->get_int(ARG_TARGET_MAX_DISTANCE)) {
            errs->push_back(new error(ERR_TOO_FAR));
        }
    }
    
    // Check the block's requirements
    vector<requirement*>::iterator it = block->requirements->begin();
    for(; it != block->requirements->end(); ++it) {
        require_resp * reqresp = (*it)->evaluate_for(target, NULL);
        errs->insert(errs->end(), reqresp->errors->begin(), reqresp->errors->end());
    }
    
    if (errs->size() > 0) {
        return errs;
    } else {
        return NULL;
    }    
}

// Helper functions ============================

// Compare priority of two actions
bool action::priority_comp(action * a, action * b) {

    return a->priority >= b->priority;
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
