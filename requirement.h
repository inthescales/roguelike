#ifndef REQUIREMENT_H
#define REQUIREMENT_H

#include "error.h"

#include <set>
#include <string>
#include <vector>

using std::string;
using std::set;

class argmap;
class mapentity;

enum requirement_t {
    REQ_NULL = 0,
    REQ_HAS_FLAG, // vvv - simple flag or state requirements
    REQ_HAS_STAT,
    REQ_STAT_EQUALS,
    REQ_STAT_GREATER_THAN,
    REQ_STAT_LESS_THAN, // ^^^
    REQ_ACTOR_HAS_ITEMS, // vvv - checks for general actor info
    REQ_ACTOR_IS_PLAYER,
    REQ_ACTOR_CAN_WALK,
    REQ_ACTOR_CAN_WALK_TO,
    REQ_ACTOR_CAN_HOLD, // vvv - checks for actor capabilities
    REQ_ACTOR_CAN_TAKE,
    REQ_ACTOR_CAN_DROP,
    REQ_ACTOR_CAN_EQUIP,
    REQ_ACTOR_CAN_UNEQUIP,
    REQ_ACTOR_CAN_EAT,
    REQ_ACTOR_CAN_DRINK,
    REQ_ACTOR_CAN_OPEN_FEAT,
    REQ_ACTOR_CAN_CLOSE_FEAT,
    REQ_ACTOR_CAN_STRIKE,
    REQ_ACTOR_CAN_PUNCH
    
};

class requirement {

    public:
    bool negated, req_all;
    requirement_t req_type;
    string error;
    argmap * args, * externalArgs;
    
    requirement(requirement_t);
    requirement(string, requirement_t);
    requirement(string, requirement_t, bool, bool);
    
    set<error_t> * check();
    set<error_t> * check_for(entity *);
    static set<error_t> * check_requirements(vector<requirement*> *, argmap *);
    static set<error_t> * check_requirements_for(vector<requirement*> *, entity *, argmap *);
    
    void clear_external_args();
    void * get_used_arg(args_t);
    bool has_used_arg(args_t);
    vector<void*> * get_unary();
};

#endif