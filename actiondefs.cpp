#include "action.h"
#include "actiondefs.h"
#include "entity.h"

void define_actions() {

	int c = -1;

    action * new_action;
    targetActionBlock * new_target_block;
    requirementActionBlock * new_require_block;
    effectActionBlock * new_effect_block;
    argmap * new_effect_args;
    
    //#WALK_BASIC
    new_action = new action();
    new_target_block = new targetActionBlock("Walk where?", TAR_ADJ, RAD_SINGLE, ACTROLE_PATIENT);
    new_target_block->args->add_int(ARG_TARGET_NUMBER, 1);
    new_target_block->args->add_int(ARG_TARGET_ENTITY_TYPE, ENT_TYPE_TILE);
    new_action->add_block(new_target_block);
    new_require_block = new requirementActionBlock(false, false);
    new_require_block->requirements->push_back(new requirement("You can't walk there.", REQ_ACTOR_CAN_WALK));
    new_action->add_block(new_require_block);
    new_action->add_block(new effectActionBlock( new effect(EFF_WALK)));
    new_action->purpose = ACTPUR_MOVE;
    actiondef[++c] = new_action;
    
    //#EAT_BASIC
    // target inv(obj.1), require item can be eaten, effect eating on item
    new_action = new action();
    new_target_block = new targetActionBlock("Eat what?", TAR_INV, RAD_SINGLE, ACTROLE_PATIENT);
    new_target_block->args->add_int(ARG_TARGET_NUMBER, 1);
    new_target_block->args->add_int(ARG_TARGET_ENTITY_TYPE, ENT_TYPE_OBJECT);
    new_action->add_block(new_target_block);
    new_require_block = new requirementActionBlock(false, false);
    new_require_block->requirements->push_back(new requirement("You can't eat that.", REQ_ACTOR_CAN_EAT));
    new_action->add_block(new_require_block);
    new_action->add_block(new effectActionBlock(new effect(EFF_EAT)));
    new_action->purpose = ACTPUR_EAT;
    actiondef[++c] = new_action;
    
    //#DRINK_BASIC
    // target inv(obj.1), require item can be drunk, effect drinking item
    new_action = new action();
    new_target_block = new targetActionBlock("Drink what?", TAR_INV, RAD_SINGLE, ACTROLE_PATIENT);
    new_target_block->args->add_int(ARG_TARGET_NUMBER, 1);
    new_target_block->args->add_int(ARG_TARGET_ENTITY_TYPE, ENT_TYPE_OBJECT);
    new_action->add_block(new_target_block);
    new_require_block = new requirementActionBlock(false, false);
    new_require_block->requirements->push_back(new requirement("You can't drink that.", REQ_ACTOR_CAN_DRINK));
    new_action->add_block(new_require_block);
    new_action->add_block(new effectActionBlock( new effect(EFF_DRINK)));
    new_action->purpose = ACTPUR_DRINK;
    actiondef[++c] = new_action;

    //#OPEN_BASIC
    // target adj(tile.1), 
    new_action = new action();
    new_target_block = new targetActionBlock("Open what?", TAR_ADJ, RAD_SINGLE, ACTROLE_PATIENT);
    new_target_block->args->add_int(ARG_TARGET_NUMBER, 1);
    new_target_block->args->add_int(ARG_TARGET_DISTANCE, 1);
    new_target_block->args->add_int(ARG_TARGET_ENTITY_TYPE, ENT_TYPE_FEATURE);
    new_action->add_block(new_target_block);
    new_require_block = new requirementActionBlock(false, false);
    new_require_block->requirements->push_back(new requirement("You can't open that.", REQ_ACTOR_CAN_OPEN_FEAT));
    new_action->add_block(new_require_block);
    new_action->add_block(new effectActionBlock(new effect(EFF_FEAT_OPEN)));
    new_action->purpose = ACTPUR_OPEN_FEAT;
    new_action->priority = 10;
    new_action->contextOk = true;
    actiondef[++c] = new_action;
    
    //#CLOSE_BASIC
    // target adj(tile.1), 
    new_action = new action();
    new_target_block = new targetActionBlock("Close what?", TAR_ADJ, RAD_SINGLE, ACTROLE_PATIENT);
    new_target_block->args->add_int(ARG_TARGET_NUMBER, 1);
    new_target_block->args->add_int(ARG_TARGET_DISTANCE, 1);
    new_target_block->args->add_int(ARG_TARGET_ENTITY_TYPE, ENT_TYPE_FEATURE);
    new_action->add_block(new_target_block);
    new_require_block = new requirementActionBlock(false, false);
    new_require_block->requirements->push_back(new requirement("You can't close that.", REQ_ACTOR_CAN_CLOSE_FEAT));
    new_action->add_block(new_require_block);
    new_action->add_block(new effectActionBlock(new effect(EFF_FEAT_CLOSE)));
    new_action->purpose = ACTPUR_CLOSE_FEAT;
    new_action->priority = 10;
    new_action->contextOk = true;
    actiondef[++c] = new_action;

    //#STRIKE
    // target adj(tile.1), 
    new_action = new action();
    new_target_block = new targetActionBlock("Attack what?", TAR_ADJ, RAD_SINGLE, ACTROLE_PATIENT);
    new_target_block->args->add_int(ARG_TARGET_NUMBER, 1);
    new_target_block->args->add_int(ARG_TARGET_DISTANCE, 1);
    new_target_block->args->add_int(ARG_TARGET_ENTITY_TYPE, ENT_TYPE_ACTOR);
    new_action->add_block(new_target_block);
    new_require_block = new requirementActionBlock(false, false);
    new_require_block->requirements->push_back(new requirement("", REQ_ACTOR_CAN_STRIKE));
    new_action->add_block(new_require_block);
    new_action->add_block(new effectActionBlock(new effect(EFF_STRIKE)));
    new_action->purpose = ACTPUR_HARM;
    new_action->priority = 10;
    new_action->contextOk = true;
    actiondef[++c] = new_action;    
}