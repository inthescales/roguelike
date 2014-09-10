#include "display.h"
#include "objclass.h"
#include "objdefs.h"
#include "globals.h"
#include "effect.h"

#include <utility>

using std::pair;
using std::vector;

void define_objects(){

	int c = -1;
	
	//#BREAD_RATION
	oclass[++c] = new objclass("bread ration", 165, C_BROWN, OT_FOOD, OST_FOOD, MAT_VEGETATION, 1, 5);
	oclass[c]->add_effect(trigger_effect(TRG_EAT, effect(TAR_SELF, RAD_SINGLE, EFF_BREAD)));
	oclass[c]->stats->add_stat(OSTAT_NUTRITION, 2);
	
	//#DAGGER	
	oclass[++c] = new objclass("dagger", '(', C_GRAY, OT_WEAPON, OST_KNIFE, MAT_METAL, 2, 8);
	oclass[c]->stats->add_stat(OSTAT_WEP_DMG, 5);
	
	//#PLUMED_CAP	
	oclass[++c] = new objclass("plumed cap", '[', C_RED, OT_ARMOR, OST_HAT, MAT_CLOTH, 4, 12);
	oclass[c]->stats->add_stat(OSTAT_ARM_BLOCK, 1);
	
	//#GOLD_COIN
	oclass[++c] = new objclass("gold coin", '$', C_YELLOW, OT_TREASURE, OST_COIN, MAT_METAL, 1, 1);
	
}

/*
	Idea - have the code be able to read from a data file, or load from code like now.
	Then, have a script that converts data files of that same format into code of this
	format. Distribute with the compiled-in version, but allow a setting that points to
	a mod directory, and loads data files at runtime from there.
*/
