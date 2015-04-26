#ifndef EFFECT_H

#define EFFECT_H

#include "argmap.h"
#include "enums.h"
#include <string>

using std::string;

class actor;
class object;
class tile;
class feature;

//The actual effect that occurs
enum effect_t {
    EFF_TURN = 0,
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

class effect {

	public:
	effect_t type;
    argmap * args;
    
    effect(effect_t);
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

bool do_effect(argmap *, effect *);

#endif
