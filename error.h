#ifndef ERROR_H
#define ERROR_H

#include "argmap.h"
#include <string>
#include <vector>

using std::string;
using std::vector;

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

class error {

    public:
    error_t code;
    string str;
    argmap * args;
    
    error(error_t);
    error(error_t, string);
    error(error_t, string, argmap *);
    
    static bool contains_error(vector<error*> *, error_t);
};

extern string error_string[ERR_MAX];

void define_errors();

#endif