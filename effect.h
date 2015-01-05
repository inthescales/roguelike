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
