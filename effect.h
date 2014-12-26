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

class effect {

	public:
    effect(effect_t);
	effect(radius_t, effect_t);

	radius_t radius;
	effect_t type;
};

class trigger_effect {
	
	public:
	trigger_effect(trigger_t, effect *);
	
	trigger_t trigger;
	effect * eff;
};

/*
 A combination of an effect and the numbers necessary to create
 a timer for it
*/
class timer_effect {

    public:
    timer_effect(int, int, int, effect *);
    
    int time;
    int iterations;
    int delta;
    
    effect * eff;
};

bool do_effect(argmap *, effect *);

#endif
