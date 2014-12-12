#include "action.h"

// Actionblock stuff ===================

targetActionBlock::targetActionBlock(argmap * nargs, target_t ntarg, radius_t nrad, args_t npos) {

    args = nargs;
    target_type = ntarg;
    radius_type = nrad;
    position = npos;
    block_type = TARGET_BLOCK;
}

effectActionBlock::effectActionBlock(argmap * nargs, effect_t neff) {

    args = nargs;
    effect_type = neff;
    block_type = EFFECT_BLOCK;
}

// Action itself =====================

action::action(argmap * nreqs, vector<actionBlock *> * nblocks) {

    requirements = nreqs;
    blocks = nblocks;
}

void action::execute() {


}
