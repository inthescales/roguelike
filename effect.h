#ifndef EFFECT_H

#define EFFECT_H

#include "argmap.h"
#include "enums.h"
#include <string>

using std::string;

class actor;
class error;
class object;
class tile;
class feature;

//The actual effect that occurs
enum effect_t {
    EFF_TURN = 0,
    EFF_WAIT,
    EFF_MOVE_ACT,
    EFF_TAKE,
    EFF_DROP,
    EFF_EQUIP,
    EFF_UNEQUIP,
    EFF_EAT,
    EFF_DRINK,
    EFF_STRIKE,
    EFF_PUNCH,
    EFF_COND_TIMEOUT,
	EFF_EAT_BREAD,
    EFF_DRINK_WATER,
    EFF_FEAT_OPEN,
    EFF_FEAT_CLOSE
};

enum effect_result_t {

    EFFRES_UNKNOWN = 0,
    EFFRES_SUCCESS,
    EFFRES_PARTIAL,
    EFFRES_FAILURE
};

class effect_resp {

    public:
    effect_result_t result;
    vector<void*> * successes;
    vector<error*> * errors;
    
    effect_resp();
    void merge(effect_resp *);
};

class effect {

	public:
	effect_t type;
    argmap * args;
    
    effect(effect_t);
    effect_resp * resolve();
    effect_resp * resolve(argmap *);
    static effect_resp * resolve(effect_t, argmap *, argmap *);
};

class trigger_effect : public effect {
	
	public:
    trigger_t trigger;
       
	trigger_effect(effect_t, trigger_t);
};

class timer_effect : public effect {

    public:
    int time;
    int iterations;
    int delta;
    
    timer_effect(effect_t, int, int, int);
};

#endif
