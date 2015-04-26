#ifndef ERROR_H
#define ERROR_H

#include <string>

using std::string;

enum error_t {

    ERR_NONE,
    ERR_SILENT,
    ERR_GENERIC,
    ERR_FAIL,
    ERR_CANCELLED,
    
    ERR_BAD_INPUT,
    ERR_ENTITY_TYPE,
    ERR_CANT_TARGET_SELF,
    ERR_MUST_TARGET_SELF,
    ERR_TOO_FAR,
    ERR_TOO_CLOSE,
    ERR_NEED_WEAPON,
    
    ERR_CANT_MOVE_TO,
    ERR_CANT_WALK,
    ERR_CANT_CARRY,
    ERR_CANT_EQUIP,
    ERR_CANT_UNEQUIP,
    ERR_CANT_EAT,
    ERR_CANT_DRINK,
    ERR_CANT_OPEN,
    ERR_CANT_CLOSE,
    ERR_NOT_EQUIPPED,
    ERR_ALREADY_EQUIPPED,
    ERR_NO_ITEMS,
    ERR_NOTHING_TO_TAKE,
    ERR_NOTHING_TO_OPEN,
    ERR_NOTHING_TO_CLOSE,
    ERR_MAX
};

extern string error_string[ERR_MAX];

void define_errors();

#endif