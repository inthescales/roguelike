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
#define _set_require_role(p)                                             new_requirement->set_roles(p);
#define _set_require_roles(p,m)                                          new_requirement->set_roles(p,m);
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
                _set_require_roles(ARG_ACTION_AGENT, ARG_ACTION_LOCATION)
            _add_require_block_req
        _append_require
        _new_effect_block(EFF_MOVE_ACT);
            _effect_block_ends_test
        _append_effect
    _action_purpose(ACTPUR_MOVE)
    _action_priority(10)
    _action_is_context_ok
    _finish_action

    //#TAKE_BASIC
    _new_action
        _new_target_block("Take what?", TAR_SELF, RAD_SINGLE, EXT_OBJECTS, ACTROLE_PATIENT)
            _add_target_block_arg(ARG_TARGET_NUMBER, -1)
            _add_target_block_arg(ARG_TARGET_ASSUME, 1)
        _append_target
        _new_require_block(false, false, true)
            _new_requirement(REQ_ACTOR_CAN_TAKE)
                _set_require_roles(ARG_ACTION_AGENT, ARG_ACTION_PATIENT)
            _add_require_block_req
        _append_require
        _new_effect_block(EFF_TAKE)
            _effect_block_ends_test
        _append_effect
    _action_purpose(ACTPUR_TAKE)
    _action_priority(6)
    _finish_action
        
    //#DROP_BASIC
    _new_action
        _new_require_block(false, false, true)
            _new_requirement(REQ_ACTOR_HAS_ITEMS)
                _set_require_role(ARG_ACTION_AGENT)
            _add_require_block_req
        _append_require
        _new_target_block("Drop what?", TAR_SELF, RAD_SINGLE, EXT_INVENTORY, ACTROLE_PATIENT)
            _add_target_block_arg(ARG_TARGET_NUMBER, -1)
        _append_target
        _new_require_block(false, false, true)
            _new_requirement(REQ_ACTOR_CAN_DROP)
                _set_require_roles(ARG_ACTION_AGENT, ARG_ACTION_PATIENT)
            _add_require_block_req
        _append_require
        _new_effect_block(EFF_DROP)
            _effect_block_ends_test
        _append_effect
    _action_purpose(ACTPUR_DROP)
    _action_priority(10)
    _finish_action
    
    //#EQUIP_BASIC
    // target inv(obj.1), require item can be eaten, effect eating on item
    _new_action
        _new_require_block(false, false, true)
            _new_requirement(REQ_ACTOR_HAS_ITEMS)
                _set_require_role(ARG_ACTION_AGENT)
            _add_require_block_req
        _append_require
        _new_target_block("Equip what?", TAR_SELF, RAD_SINGLE, EXT_INVENTORY, ACTROLE_PATIENT)
            _add_target_block_arg(ARG_TARGET_NUMBER, 1)
        _append_target
        _new_require_block(false, false, true);
            _new_requirement(REQ_ACTOR_CAN_EQUIP)
                _set_require_roles(ARG_ACTION_AGENT, ARG_ACTION_PATIENT)
            _add_require_block_req
        _append_require
        _new_effect_block(EFF_EQUIP)
            _effect_block_ends_test
        _append_effect
    _action_purpose(ACTPUR_EQUIP)
    _action_priority(10)
    _finish_action
    
    //#UNEQUIP_BASIC
    // target inv(obj.1), require item can be eaten, effect eating on item
    _new_action
        _new_require_block(false, false, true)
            _new_requirement(REQ_ACTOR_HAS_ITEMS)
                _set_require_role(ARG_ACTION_AGENT)
            _add_require_block_req
        _append_require
        _new_target_block("Unequip what?", TAR_SELF, RAD_SINGLE, EXT_INVENTORY, ACTROLE_PATIENT)
            _add_target_block_arg(ARG_TARGET_NUMBER, 1)
        _append_target
        _new_require_block(false, false, true);
            _new_requirement(REQ_ACTOR_CAN_UNEQUIP)
                _set_require_roles(ARG_ACTION_AGENT, ARG_ACTION_PATIENT)
            _add_require_block_req
        _append_require
        _new_effect_block(EFF_UNEQUIP)
            _effect_block_ends_test
        _append_effect
    _action_purpose(ACTPUR_UNEQUIP)
    _action_priority(10)
    _finish_action   
    
    //#EAT_BASIC
    // target inv(obj.1), require item can be eaten, effect eating on item
    _new_action
        _new_require_block(false, false, true)
            _new_requirement(REQ_ACTOR_HAS_ITEMS)
                _set_require_role(ARG_ACTION_AGENT)
            _add_require_block_req
        _append_require
        _new_target_block("Eat what?", TAR_SELF, RAD_SINGLE, EXT_INVENTORY, ACTROLE_PATIENT)
            _add_target_block_arg(ARG_TARGET_NUMBER, 1)
        _append_target
        _new_require_block(false, false, true);
            _new_requirement(REQ_ACTOR_CAN_EAT)
                _set_require_roles(ARG_ACTION_AGENT, ARG_ACTION_PATIENT)
            _add_require_block_req
        _append_require
        _new_effect_block(EFF_EAT)
            _effect_block_ends_test
        _append_effect
    _action_purpose(ACTPUR_EAT)
    _action_priority(10)
    _finish_action
    
    //#DRINK_BASIC
    // target inv(obj.1), require item can be drunk, effect drinking item
    _new_action
        _new_require_block(false, false, true)
            _new_requirement(REQ_ACTOR_HAS_ITEMS)
                _set_require_role(ARG_ACTION_AGENT)
            _add_require_block_req
        _append_require
        _new_target_block("Drink what?", TAR_SELF, RAD_SINGLE, EXT_INVENTORY, ACTROLE_PATIENT)
            _add_target_block_arg(ARG_TARGET_NUMBER, 1)
        _append_target
        _new_require_block(false, false, true);
            _new_requirement(REQ_ACTOR_CAN_DRINK)
                _set_require_roles(ARG_ACTION_AGENT, ARG_ACTION_PATIENT)
            _add_require_block_req
        _append_require
        _new_effect_block(EFF_DRINK)
            _effect_block_ends_test
        _append_effect
    _action_purpose(ACTPUR_DRINK)
    _action_priority(10)
    _finish_action

    //#OPEN_BASIC
    // target adj(tile.1), 
    _new_action
        _new_target_block("Open what?", TAR_ADJ, RAD_SINGLE, EXT_FEATURE, ACTROLE_PATIENT)
            _add_target_block_arg(ARG_TARGET_NUMBER, 1)
            _add_target_block_arg(ARG_TARGET_ASSUME, 1)
            _add_target_block_arg(ARG_TARGET_MIN_DISTANCE, 1)
            _add_target_block_arg(ARG_TARGET_MAX_DISTANCE, 1)
        _append_target
        _new_require_block(false, false, true)
            _new_requirement(REQ_ACTOR_CAN_OPEN_FEAT)
                _set_require_roles(ARG_ACTION_AGENT, ARG_ACTION_PATIENT)
            _add_require_block_req
        _append_require
        _new_effect_block(EFF_FEAT_OPEN)
            _effect_block_ends_test
        _append_effect
    _action_purpose(ACTPUR_OPEN_FEAT)
    _action_priority(10)
    _action_is_context_ok
    _finish_action
    
    //#CLOSE_BASIC
    // target adj(tile.1),
    _new_action
        _new_target_block("Close what?", TAR_ADJ, RAD_SINGLE, EXT_FEATURE, ACTROLE_PATIENT)
            _add_target_block_arg(ARG_TARGET_NUMBER, 1)
            _add_target_block_arg(ARG_TARGET_ASSUME, 1)
            _add_target_block_arg(ARG_TARGET_MIN_DISTANCE, 1)
            _add_target_block_arg(ARG_TARGET_MAX_DISTANCE, 1)
        _append_target
        _new_require_block(false, false, true)
            _new_requirement(REQ_ACTOR_CAN_CLOSE_FEAT)
                _set_require_roles(ARG_ACTION_AGENT, ARG_ACTION_PATIENT)
            _add_require_block_req
        _append_require
        _new_effect_block(EFF_FEAT_CLOSE)
            _effect_block_ends_test
        _append_effect
    _action_purpose(ACTPUR_CLOSE_FEAT)
    _action_priority(10)
    _action_is_context_ok
    _finish_action

    //#STRIKE
    // target adj(tile.1), 
    _new_action
        _new_target_block("Attack what?", TAR_ADJ, RAD_SINGLE, EXT_ACTOR, ACTROLE_PATIENT)
            _add_target_block_arg(ARG_TARGET_NUMBER, 1)
            _add_target_block_arg(ARG_TARGET_MIN_DISTANCE, 1)
            _add_target_block_arg(ARG_TARGET_MAX_DISTANCE, 1)
        _append_target
        _new_require_block(false, false, true)
            _new_requirement(REQ_ACTOR_CAN_STRIKE)
                _set_require_roles(ARG_ACTION_AGENT, ARG_ACTION_PATIENT)
            _add_require_block_req
        _append_require
        _new_effect_block(EFF_STRIKE)
            _effect_block_ends_test
        _append_effect
    _action_purpose(ACTPUR_HARM)
    _action_priority(10)
    _finish_action
    
    //#PUNCH
    // target adj(tile.1), 
    _new_action
        _new_target_block("Attack what?", TAR_ADJ, RAD_SINGLE, EXT_ACTOR, ACTROLE_PATIENT)
            _add_target_block_arg(ARG_TARGET_NUMBER, 1)
            _add_target_block_arg(ARG_TARGET_MIN_DISTANCE, 1)
            _add_target_block_arg(ARG_TARGET_MAX_DISTANCE, 1)
        _append_target
        _new_require_block(false, false, true)
            _new_requirement(REQ_ACTOR_CAN_PUNCH)
                _set_require_roles(ARG_ACTION_AGENT, ARG_ACTION_PATIENT)
            _add_require_block_req
        _append_require
        _new_effect_block(EFF_PUNCH)
            _effect_block_ends_test
        _append_effect
    _action_purpose(ACTPUR_HARM)
    _action_priority(5)
    _finish_action
    
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