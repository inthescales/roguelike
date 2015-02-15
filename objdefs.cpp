#include "action.h"
#include "display.h"
#include "objclass.h"
#include "objdefs.h"
#include "object.h"
#include "globals.h"
#include "effect.h"
#include "symboldefs.h"

#include <utility>

using std::pair;
using std::vector;

void define_objects() {

	int c = -1;
	
    effect * new_effect;
    
	//#BREAD_RATION
	oclass[++c] = new objclass("bread ration", symboldef[SYM_FOOD], C_BROWN, OT_FOOD, OST_FOOD, MAT_VEGETATION, 1, 5);
    new_effect = new trigger_effect(EFF_EAT_BREAD, TRG_EAT);
	oclass[c]->add_trigger_effect((trigger_effect *)new_effect);
	oclass[c]->add_stat(OSTAT_NUTRITION, 2);
    
    //#BOTTLE_WATER
	oclass[++c] = new objclass("bottle of water", symboldef[SYM_DRINK], C_BLUE, OT_DRINK, OST_POTION, MAT_GLASS, 1, 5);
	new_effect = new trigger_effect(EFF_DRINK_WATER, TRG_DRINK);
    oclass[c]->add_trigger_effect((trigger_effect *)new_effect);
	oclass[c]->add_stat(OSTAT_NUTRITION, 1);
	
	//#DAGGER	
	oclass[++c] = new objclass("dagger", symboldef[SYM_WEAPON], C_GRAY, OT_WEAPON, OST_KNIFE, MAT_METAL, 2, 8);
	oclass[c]->add_stat(OSTAT_WEP_DMG, 5);
	
	//#PLUMED_CAP	
	oclass[++c] = new objclass("plumed cap", symboldef[SYM_ARMOR], C_RED, OT_ARMOR, OST_HAT, MAT_CLOTH, 4, 12);
	oclass[c]->add_stat(OSTAT_ARM_BLOCK, 1);
    oclass[c]->add_action(ACTION_OPEN_BASIC);
	
	//#GOLD_COIN
	oclass[++c] = new objclass("gold coin", symboldef[SYM_MONEY], C_YELLOW, OT_TREASURE, OST_COIN, MAT_METAL, 1, 1);
	
}

/*
	Idea - have the code be able to read from a data file, or load from code like now.
	Then, have a script that converts data files of that same format into code of this
	format. Distribute with the compiled-in version, but allow a setting that points to
	a mod directory, and loads data files at runtime from there.
*/
