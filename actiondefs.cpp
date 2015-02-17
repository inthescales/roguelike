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
    new_target_block = new targetActionBlock("Walk where?", TAR_ADJ, RAD_SINGLE, EXT_TILE, ACTROLE_PATIENT);
    new_target_block->args->add_int(ARG_TARGET_NUMBER, 1);
    new_action->add_block(new_target_block);
    new_require_block = new requirementActionBlock(false, false, true);
    new_require_block->requirements->push_back(new requirement(REQ_ACTOR_CAN_WALK));
    new_action->add_block(new_require_block);
    new_action->add_block(new effectActionBlock( new effect(EFF_WALK)));
    new_action->purpose = ACTPUR_MOVE;
    new_action->contextOk = true;
    actiondef[++c] = new_action;
    
    //#TAKE_BASIC
    new_action = new action();
    new_target_block = new targetActionBlock("Take what?", TAR_SELF, RAD_SINGLE, EXT_OBJECTS, ACTROLE_PATIENT);
    new_target_block->args->add_int(ARG_TARGET_NUMBER, -1);
    new_target_block->args->add_int(ARG_TARGET_ASSUME, 1);
    new_action->add_block(new_target_block);
    new_require_block = new requirementActionBlock(false, false, true);
    new_require_block->requirements->push_back(new requirement(REQ_ACTOR_CAN_TAKE));
    new_action->add_block(new_require_block);
    new_action->add_block(new effectActionBlock(new effect(EFF_TAKE)));
    new_action->purpose = ACTPUR_TAKE;
    actiondef[++c] = new_action;
    
    //#DROP_BASIC
    new_action = new action();
    new_require_block = new requirementActionBlock(false, false, true);
    new_require_block->requirements->push_back(new requirement(REQ_ACTOR_HAS_ITEMS));
    new_action->add_block(new_require_block);
    new_target_block = new targetActionBlock("Drop what?", TAR_SELF, RAD_SINGLE, EXT_INVENTORY, ACTROLE_PATIENT);
    new_target_block->args->add_int(ARG_TARGET_NUMBER, -1);
    new_action->add_block(new_target_block);
    new_require_block = new requirementActionBlock(false, false, true);
    new_require_block->requirements->push_back(new requirement(REQ_ACTOR_CAN_DROP));
    new_action->add_block(new_require_block);
    new_action->add_block(new effectActionBlock(new effect(EFF_DROP)));
    new_action->purpose = ACTPUR_DROP;
    actiondef[++c] = new_action;
    
    //#EQUIP_BASIC
    // target inv(obj.1), require item can be eaten, effect eating on item
    new_action = new action();
    new_require_block = new requirementActionBlock(false, false, true);
    new_require_block->requirements->push_back(new requirement(REQ_ACTOR_HAS_ITEMS));
    new_action->add_block(new_require_block);
    new_target_block = new targetActionBlock("Equip what?", TAR_SELF, RAD_SINGLE, EXT_INVENTORY, ACTROLE_PATIENT);
    new_target_block->args->add_int(ARG_TARGET_NUMBER, 1);
    new_action->add_block(new_target_block);
    new_require_block = new requirementActionBlock(false, false, true);
    new_require_block->requirements->push_back(new requirement(REQ_ACTOR_CAN_EQUIP));
    new_action->add_block(new_require_block);
    new_action->add_block(new effectActionBlock(new effect(EFF_EQUIP)));
    new_action->purpose = ACTPUR_EQUIP;
    actiondef[++c] = new_action;
    
    //#UNEQUIP_BASIC
    // target inv(obj.1), require item can be eaten, effect eating on item
    new_action = new action();
    new_require_block = new requirementActionBlock(false, false, true);
    new_require_block->requirements->push_back(new requirement(REQ_ACTOR_HAS_ITEMS));
    new_action->add_block(new_require_block);
    new_target_block = new targetActionBlock("Unequip what?", TAR_SELF, RAD_SINGLE, EXT_INVENTORY, ACTROLE_PATIENT);
    new_target_block->args->add_int(ARG_TARGET_NUMBER, 1);
    new_action->add_block(new_target_block);
    new_require_block = new requirementActionBlock(false, false, true);
    new_require_block->requirements->push_back(new requirement(REQ_ACTOR_CAN_UNEQUIP));
    new_action->add_block(new_require_block);
    new_action->add_block(new effectActionBlock(new effect(EFF_UNEQUIP)));
    new_action->purpose = ACTPUR_EQUIP;
    actiondef[++c] = new_action;
    
    
    //#EAT_BASIC
    // target inv(obj.1), require item can be eaten, effect eating on item
    new_action = new action();
    new_require_block = new requirementActionBlock(false, false, true);
    new_require_block->requirements->push_back(new requirement(REQ_ACTOR_HAS_ITEMS));
    new_action->add_block(new_require_block);
    new_target_block = new targetActionBlock("Eat what?", TAR_SELF, RAD_SINGLE, EXT_INVENTORY, ACTROLE_PATIENT);
    new_target_block->args->add_int(ARG_TARGET_NUMBER, 1);
    new_action->add_block(new_target_block);
    new_require_block = new requirementActionBlock(false, false, true);
    new_require_block->requirements->push_back(new requirement(REQ_ACTOR_CAN_EAT));
    new_action->add_block(new_require_block);
    new_action->add_block(new effectActionBlock(new effect(EFF_EAT)));
    new_action->purpose = ACTPUR_EAT;
    actiondef[++c] = new_action;
    
    //#DRINK_BASIC
    // target inv(obj.1), require item can be drunk, effect drinking item
    new_action = new action();
    new_require_block = new requirementActionBlock(false, false, true);
    new_require_block->requirements->push_back(new requirement(REQ_ACTOR_HAS_ITEMS));
    new_action->add_block(new_require_block);
    new_target_block = new targetActionBlock("Drink what?", TAR_SELF, RAD_SINGLE, EXT_INVENTORY, ACTROLE_PATIENT);
    new_target_block->args->add_int(ARG_TARGET_NUMBER, 1);
    new_action->add_block(new_target_block);
    new_require_block = new requirementActionBlock(false, false, true);
    new_require_block->requirements->push_back(new requirement(REQ_ACTOR_CAN_DRINK));
    new_action->add_block(new_require_block);
    new_action->add_block(new effectActionBlock( new effect(EFF_DRINK)));
    new_action->purpose = ACTPUR_DRINK;
    actiondef[++c] = new_action;

    //#OPEN_BASIC
    // target adj(tile.1), 
    new_action = new action();
    new_target_block = new targetActionBlock("Open what?", TAR_ADJ, RAD_SINGLE, EXT_FEATURE, ACTROLE_PATIENT);
    new_target_block->args->add_int(ARG_TARGET_NUMBER, 1);
    new_target_block->args->add_int(ARG_TARGET_DISTANCE, 1);
    new_action->add_block(new_target_block);
    new_require_block = new requirementActionBlock(false, false, true);
    new_require_block->requirements->push_back(new requirement(REQ_ACTOR_CAN_OPEN_FEAT));
    new_action->add_block(new_require_block);
    new_action->add_block(new effectActionBlock(new effect(EFF_FEAT_OPEN)));
    new_action->purpose = ACTPUR_OPEN_FEAT;
    new_action->priority = 10;
    new_action->contextOk = true;
    actiondef[++c] = new_action;
    
    //#CLOSE_BASIC
    // target adj(tile.1), 
    new_action = new action();
    new_target_block = new targetActionBlock("Close what?", TAR_ADJ, RAD_SINGLE, EXT_FEATURE, ACTROLE_PATIENT);
    new_target_block->args->add_int(ARG_TARGET_NUMBER, 1);
    new_target_block->args->add_int(ARG_TARGET_DISTANCE, 1);
    new_action->add_block(new_target_block);
    new_require_block = new requirementActionBlock(false, false, true);
    new_require_block->requirements->push_back(new requirement(REQ_ACTOR_CAN_CLOSE_FEAT));
    new_action->add_block(new_require_block);
    new_action->add_block(new effectActionBlock(new effect(EFF_FEAT_CLOSE)));
    new_action->purpose = ACTPUR_CLOSE_FEAT;
    new_action->priority = 10;
    new_action->contextOk = true;
    actiondef[++c] = new_action;

    //#STRIKE
    // target adj(tile.1), 
    new_action = new action();
    new_target_block = new targetActionBlock("Attack what?", TAR_ADJ, RAD_SINGLE, EXT_ACTOR, ACTROLE_PATIENT);
    new_target_block->args->add_int(ARG_TARGET_NUMBER, 1);
    new_target_block->args->add_int(ARG_TARGET_DISTANCE, 1);
    new_action->add_block(new_target_block);
    new_require_block = new requirementActionBlock(false, false, true);
    new_require_block->requirements->push_back(new requirement(REQ_ACTOR_CAN_STRIKE));
    new_action->add_block(new_require_block);
    new_action->add_block(new effectActionBlock(new effect(EFF_STRIKE)));
    new_action->purpose = ACTPUR_HARM;
    new_action->priority = 10;
    new_action->contextOk = true;
    actiondef[++c] = new_action;
    
    //#PUNCH
    // target adj(tile.1), 
    new_action = new action();
    new_target_block = new targetActionBlock("Attack what?", TAR_ADJ, RAD_SINGLE, EXT_ACTOR, ACTROLE_PATIENT);
    new_target_block->args->add_int(ARG_TARGET_NUMBER, 1);
    new_target_block->args->add_int(ARG_TARGET_DISTANCE, 1);
    new_action->add_block(new_target_block);
    new_require_block = new requirementActionBlock(false, false, true);
    new_require_block->requirements->push_back(new requirement(REQ_ACTOR_CAN_PUNCH));
    new_action->add_block(new_require_block);
    new_action->add_block(new effectActionBlock(new effect(EFF_PUNCH)));
    new_action->purpose = ACTPUR_HARM;
    new_action->priority = 5;
    new_action->contextOk = true;
    actiondef[++c] = new_action;
    
    //#POINT
    // target adj(tile.1), 
    new_action = new action();
    new_target_block = new targetActionBlock("Choose a point!!", TAR_TILE, RAD_SINGLE, EXT_TILE, ACTROLE_PATIENT);
    new_target_block->args->add_int(ARG_TARGET_NUMBER, 1);
    new_target_block->args->add_int(ARG_TARGET_DISTANCE, 5);
    new_action->add_block(new_target_block);
    //new_action->purpose = ACTPUR_HARM;
    new_action->priority = 10;
    new_action->contextOk = true;
    actiondef[++c] = new_action;
    
    //#LINE
    // target adj(tile.1), 
    new_action = new action();
    new_target_block = new targetActionBlock("Draw a line!", TAR_TILE, RAD_LINE, EXT_TILE, ACTROLE_PATIENT);
    new_target_block->args->add_int(ARG_TARGET_NUMBER, 10);
    new_target_block->args->add_int(ARG_TARGET_DISTANCE, 5);
    new_action->add_block(new_target_block);
    //new_action->purpose = ACTPUR_HARM;
    new_action->priority = 10;
    new_action->contextOk = true;
    actiondef[++c] = new_action;

}