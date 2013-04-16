#include "objclass.h"
#include "globals.h"
#include "graphics.h"

#define OBJ( name, symbol, color, type, material, weight, value) \
	{ name, "", symbol, color, 0, type, material, weight, value }

objclass oclass[] = {

	OBJ("bread ration", '%', C_BROWN, OT_FOOD, MAT_VEGETATION, 1, 5),
	OBJ("dagger", '(', C_GRAY, OT_WEAPON, MAT_METAL, 2, 8),
	OBJ("gold coin", '$', C_YELLOW, OT_COIN, MAT_METAL, 1, 1)
	
};
