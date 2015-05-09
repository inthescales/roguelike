#include "error.h"

// Error class =====================================================

error::error(error_t nc) {

    code = nc;
    str = "";
    args = new argmap();
}

error::error(error_t nc, string ns) {

    code = nc;
    str = ns;
    args = new argmap();
}

error::error(error_t nc, string ns, argmap * narg) {

    code = nc;
    str = ns;
    args = narg;
}

// Static functions =============================================

bool error::contains_error(vector<error*> * errors, error_t code) {

    vector<error*>::iterator it = errors->begin();
    for (; it != errors->end(); ++it) {
        
        if ((*it)->code == code) {
            return true;
        }
    }
    
    return false;
}

// Error string defs ====================================================================

string error_string[ERR_MAX];

void define_errors() {

    error_string[ERR_NONE] = "";
    error_string[ERR_SILENT] = "";
    error_string[ERR_GENERIC] = "GENERIC ERROR";
    error_string[ERR_BAD_INPUT] = "BAD INPUT ERROR";
    error_string[ERR_FAIL] = "ERROR: FAILED";
    error_string[ERR_ENTITY_TYPE] = "WRONG ENTITY TYPE";
    error_string[ERR_CANT_TARGET_SELF] = "You cannot target yourself.";
    error_string[ERR_MUST_TARGET_SELF] = "You can only target yourself.";
    error_string[ERR_TOO_FAR] = "Too far away.";
    error_string[ERR_TOO_CLOSE] = "Too close.";
    error_string[ERR_NEED_WEAPON] = "A weapon is needed.";
    error_string[ERR_CANCELLED] = "Nevermind.";
    error_string[ERR_CANT_MOVE_TO] = "You can't move there.";
    error_string[ERR_CANT_WALK] = "You aren't able to walk.";
    error_string[ERR_CANT_CARRY] = "You cannot carry any more.";
    error_string[ERR_CANT_EQUIP] = "You cannot equip that.";
    error_string[ERR_CANT_UNEQUIP] = "You cannot unequip that.";
    error_string[ERR_CANT_EAT] = "You cannot eat that.";
    error_string[ERR_CANT_DRINK] = "You cannot drink that.";
    error_string[ERR_CANT_OPEN] = "You cannot open that.";
    error_string[ERR_CANT_CLOSE] = "You cannot close that.";
    error_string[ERR_NOT_EQUIPPED] = "That item is not equipped.";
    error_string[ERR_ALREADY_EQUIPPED] = "That item is already equipped.";
    error_string[ERR_NO_ITEMS] = "You have no items.";
    error_string[ERR_NOTHING_TO_TAKE] = "There is nothing to take there.";
    error_string[ERR_NOTHING_TO_OPEN] = "There is nothing to open there.";
    error_string[ERR_NOTHING_TO_CLOSE] = "There is nothing to close there.";
};

