#ifndef REQUIREMENT_H
#define REQUIREMENT_H

#include <set>
#include <string>
#include <vector>

using std::string;
using std::set;

class argmap;
class error;
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

enum require_partial_t {

    REQPAR_PARTIAL_OK = 0,
    REQPAR_PARTIAL_SUCCEEDS,
    REQPAR_PARTIAL_FAILS
};

enum require_result_t {

    REQRES_UNKNOWN = 0,
    REQRES_SUCCESS,
    REQRES_PARTIAL,
    REQRES_FAILURE
};

class require_resp {

    public:
    require_result_t result;
    vector<void*> * successes;
    vector<error*> * errors;
    
    require_resp();
    void merge(require_resp *);
};

class requirement {

    public:
    require_partial_t partials;
    requirement_t req_type;
    string req_error_string;
    argmap * args;
    
    requirement(requirement_t);
    requirement(string, requirement_t);
    void set_roles(args_t);
    void set_roles(args_t, args_t);
    static vector<void*> * get_primary(argmap *, argmap *);
    static vector<void*> * get_secondary(argmap *, argmap *);
    
    require_resp * evaluate(argmap *);
    require_resp * evaluate_for(entity *, argmap *);
    static require_resp * evaluate_multiple(vector<requirement*> *, argmap *);
    static require_resp * evaluate_multiple_for(vector<requirement*> *, entity *, argmap *);
    static require_resp * evaluate(requirement_t, argmap *,  argmap *);
};

#endif