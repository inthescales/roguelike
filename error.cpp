#include "error.h"

string error_string[ERR_MAX];

void define_errors() {

    error_string[ERR_NONE] = "";
    error_string[ERR_SILENT] = "";
    error_string[ERR_GENERIC] = "Error.";
    error_string[ERR_CANCELLED] = "Nevermind.";
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
