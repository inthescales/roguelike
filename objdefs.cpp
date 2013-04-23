#include "objclass.h"
#include "globals.h"
#include "graphics.h"

#define OBJ( name, symbol, color, type, subtype, material, weight, value) \
	{ name, "", symbol, color, 0, type, subtype, material, weight, value }

objclass oclass[] = {

	OBJ("bread ration", '%', C_BROWN, OT_FOOD, OST_FOOD, MAT_VEGETATION, 1, 5),
	OBJ("dagger", '(', C_GRAY, OT_WEAPON, OST_KNIFE, MAT_METAL, 2, 8),
	OBJ("plumed cap", '[', C_RED, OT_ARMOR, OST_HAT, MAT_CLOTH, 4, 12),
	OBJ("gold coin", '$', C_YELLOW, OT_TREASURE, OST_COIN, MAT_METAL, 1, 1)
	
};
