#include "action.h"
#include "actiondefs.h"
#include "entity.h"

// A bunch of helpful macros

#define _new_action new_action = new action();

#define _new_target_block(prompt, type, radius, extract, role)           new_target_block = new targetActionBlock(prompt,type,radius,extract,role);
#define _add_target_block_arg(code,val)                                  new_target_block->args->add_int((args_t)code,(int)val);
#define _add_target_block_flag(code)                                     new_target_block->args->add_flag(code);
#define _add_target_block_req                                            new_target_block->requirements->push_back(new_requirement);
#define _append_target                                                   new_action->add_block(new_target_block);

#define _new_require_block(loop,end,critical)                            new_require_block = new requirementActionBlock(loop,end,critical);
#define _add_require_block_arg(code,val)                                 new_require_block->args->add_int((args_t)code,(int)val);
#define _new_requirement(type)                                           new_requirement = new requirement(type);
#define _add_requirement_arg(code,val)                                   new_requirement->args->add_int((args_t)code,(int)val);
#define _add_require_block_req                                           new_require_block->requirements->push_back(new_requirement);
#define _append_require                                                  new_action->add_block(new_require_block);

#define _new_extract_block(type,from,to)                                 new_extract_block = new extractActionBlock(type,from,to);
#define _append_extract                                                  new_action->add_block(new_extract_block);

#define _new_effect_block(eff)                                           new_effect_block = new effectActionBlock(new effect(eff));
#define _effect_block_ends_test                                          new_effect_block->testDone = true;
#define _new_effect(type)                                                new_effect = new effect(type);
#define _add_effect_arg(code,val)                                        new_effect->args->add_int((args_t)code,(int)val);
#define _append_effect                                                   new_action->add_block(new_effect_block);

#define _action_purpose(purp)                                            new_action->purpose = purp;
#define _action_priority(pri)                                            new_action->priority = pri;
#define _action_is_context_ok                                            new_action->contextOk = true;
#define _finish_action                                                   actiondef[++c] = new_action;

void define_actions() {

	int c = -1;

    action * new_action;
    targetActionBlock * new_target_block;
    extractActionBlock * new_extract_block;
    requirementActionBlock * new_require_block;
    effectActionBlock * new_effect_block;
    effect * new_effect;
    requirement * new_requirement;
    argmap * new_effect_args;
    set<int> * new_set;
    
    //#WALK_BASIC    
    _new_action
        _new_target_block("Walk where?", TAR_ADJ, RAD_SINGLE, EXT_TILE, ACTROLE_LOCATION)
            _add_target_block_arg(ARG_TARGET_NUMBER, 1)
            _add_target_block_arg(ARG_TARGET_MIN_DISTANCE, 1)
            _add_target_block_arg(ARG_TARGET_MAX_DISTANCE, 1)
        _append_target
        _new_require_block(false, false, true)
            _new_requirement(REQ_ACTOR_CAN_WALK_TO)
            _add_require_block_req
        _append_require
        _new_extract_block(EXT_COPY, ACTROLE_AGENT, ACTROLE_PATIENT)
        _append_extract
        _new_effect_block(EFF_MOVE_ACT);
            _effect_block_ends_test
        _append_effect
    _action_purpose(ACTPUR_MOVE)
    _action_priority(10)
    _action_is_context_ok
    _finish_action

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
    new_target_block->args->add_int(ARG_TARGET_ASSUME, 1);
    new_target_block->args->add_int(ARG_TARGET_MIN_DISTANCE, 1);
    new_target_block->args->add_int(ARG_TARGET_MAX_DISTANCE, 1);
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
    new_target_block->args->add_int(ARG_TARGET_ASSUME, 1);
    new_target_block->args->add_int(ARG_TARGET_MIN_DISTANCE, 1);
    new_target_block->args->add_int(ARG_TARGET_MAX_DISTANCE, 1);
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
    new_target_block->args->add_int(ARG_TARGET_MIN_DISTANCE, 1);
    new_target_block->args->add_int(ARG_TARGET_MAX_DISTANCE, 1);
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
    new_target_block->args->add_int(ARG_TARGET_MIN_DISTANCE, 1);
    new_target_block->args->add_int(ARG_TARGET_MAX_DISTANCE, 1);
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
    new_target_block->args->add_int(ARG_TARGET_MIN_DISTANCE, 1);
    new_target_block->args->add_int(ARG_TARGET_MAX_DISTANCE, 5);
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
    new_target_block->args->add_int(ARG_TARGET_MIN_DISTANCE, 1);
    new_target_block->args->add_int(ARG_TARGET_MAX_DISTANCE, 5);
    new_target_block->args->add_flag(FLAG_TARGET_BREAK_PROJECTILE);
    new_action->add_block(new_target_block);
    //new_action->purpose = ACTPUR_HARM;
    new_action->priority = 10;
    new_action->contextOk = true;
    actiondef[++c] = new_action;

}