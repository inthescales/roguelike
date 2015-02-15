#ifndef OBJECT_H
#define OBJECT_H

#include "argmap.h"
#include "config.h"
#include "display.h"
#include "effect.h"
#include "enums.h"
#include "mapentity.h"

#include <utility>
#include <string>
#include <vector>

using std::string;
using std::vector;

class condition;

//Item types
enum object_type {
	OT_FOOD = 0,
    OT_DRINK,
	OT_WEAPON,
	OT_ARMOR,
	OT_ACCESSORY,
	OT_SCROLL,
	OT_WAND,
	OT_TOOL,
	OT_TREASURE,
	OT_MISC
};

//Item subtypes
enum object_subtype {
	OST_FOOD = 0,
    OST_POTION,
	OST_SWORD,
	OST_KNIFE,
	OST_SPEAR,
	OST_AXE,
	OST_MACE,
	OST_BOW,
	OST_HAT,
	OST_HELM,
	OST_SUIT,
	OST_GLOVES,
	OST_BOOTS,
	OST_CLOAK,
	OST_RING,
	OST_AMULET,
	OST_LAMP,
	OST_COIN
};

class object : public mapentity {

	public:

    string individual_name;
	short letter;
	int quantity;
	bool equipped; //TODO - change to flag
	
	//Functions:
	object(short);
	object(short, short);
    void init();
    
    objclass * get_class();
    object_type get_type();
    object_subtype get_subtype();
    
	static bool compare_type(object *, object *);
    static bool can_stack(object *, object *);
    
	void resolve_trigger(trigger_t, argmap *);
};

#endif
