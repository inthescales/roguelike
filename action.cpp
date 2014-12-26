#include "action.h"

// Actionblock stuff ===================

targetActionBlock::targetActionBlock(string nprompt, argmap * nargs, target_t ntarg, radius_t nrad, args_t npos) {

    args = nargs;
    target_type = ntarg;
    radius_type = nrad;
    position = npos;
    prompt = nprompt;
    block_type = TARGET_BLOCK;
}

effectActionBlock::effectActionBlock(argmap * nargs, effect * neff) {

    args = nargs;
    eff = neff;
    block_type = EFFECT_BLOCK;
}

requirementActionBlock::requirementActionBlock(bool nloop, bool nend) {

    isLoop = nloop;
    endBlock = nend;
}

// Action itself =====================

action::action(argmap * nargs, vector<actionBlock *> * nblocks) {

    args = nargs;
    blocks = nblocks;
}
