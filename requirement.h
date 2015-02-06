#ifndef REQUIREMENT_H
#define REQUIREMENT_H

#include <string>
#include <vector>

using std::string;

class argmap;
class mapentity;

enum requirement_t {
    REQ_NULL = 0,
    REQ_HAS_FLAG, // vvv - simple flag or state requirements
    REQ_HAS_STAT,
    REQ_STAT_EQUALS,
    REQ_STAT_GREATER_THAN,
    REQ_STAT_LESS_THAN, // ^^^
    REQ_ACTOR_CAN_HOLD, // vvv - checks for actor capabilities
    REQ_ACTOR_CAN_WALK,
    REQ_ACTOR_CAN_SWIM,
    REQ_ACTOR_CAN_FLY,
    REQ_ACTOR_CAN_EAT,
    REQ_ACTOR_CAN_DRINK,
    REQ_ACTOR_CAN_OPEN_FEAT,
    REQ_ACTOR_CAN_CLOSE_FEAT,
    REQ_ACTOR_CAN_STRIKE
    
};

class requirement {

    public:
    bool negated;
    requirement_t req_type;
    string error;
    argmap * args;
    
    requirement(requirement_t);
    requirement(string, requirement_t);
    requirement(string, requirement_t, bool);
    bool check();
    bool check_for(mapentity *);
    
    static bool check_requirements(vector<requirement*> *);
    static bool check_requirements_for(mapentity *, vector<requirement*> *);
};

#endif