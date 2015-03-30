#include "AI.h"
#include "AIdefs.h"
#include "entity.h"

void define_AI () {

    int c = -1;

    idea * new_idea;
    
    //#ATTACK_CLOSEST
    aimod[++c] = new aimodule();
    new_idea = new idea(ENT_TYPE_ACTOR, ACTPUR_HARM, 100);
    new_idea->motivators->push_back(new motivator(false, new requirement("", REQ_ACTOR_IS_PLAYER, true), -1000));
    aimod[c]->add_idea(new_idea);
    aimod[c]->add_idea(new idea(ENT_TYPE_NONE, ACTPUR_WANDER, 50));
}
