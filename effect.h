#ifndef EFFECT_H

#define EFFECT_H

#include "enums.h"
#include "argmap.h"
#include <string>

using std::string;

class actor;
class object;
class tile;
class feature;

class effect {

	public:
    effect(effect_t);
	effect(target_t, radius_t, effect_t);
	
	target_t target;
	radius_t radius;
	effect_t type;

};

class trigger_effect {
	
	public:
	trigger_effect(trigger_t, effect);
	
	trigger_t trigger;
	effect eff;
};

bool do_effect(argmap *, effect *);

#endif
