#include "AI.h"
#include "AIdefs.h"
#include "argmap.h"
#include "entity.h"
#include "requirement.h"

void define_AI () {

    int c = -1;

    idea * new_idea;
    requirement * new_requirement;
    
    //#ATTACK_CLOSEST
    aimod[++c] = new aimodule();
    new_idea = new idea(ENT_TYPE_ACTOR, ACTPUR_HARM, 0);
    new_requirement = new requirement("", REQ_ACTOR_IS_PLAYER);
    new_requirement->args->add_int(ARG_REQUIRE_PRIMARY, ARG_ACTION_AGENT);
    new_idea->motivators->push_back(new motivator(false, new_requirement, 50));
    aimod[c]->add_idea(new_idea);
    aimod[c]->add_idea(new idea(ENT_TYPE_NONE, ACTPUR_WANDER, 50));
}
