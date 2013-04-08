#include "objclass.h"
#include "globals.h"
#include "graphics.h"

#define OBJ( name, symbol, color, type, material, weight, value) \
	{ name, "", symbol, color, 0, type, material, weight, value }

objclass oclass[] = {

	OBJ("burrito", '%', C_BROWN, OT_FOOD, MAT_VEGETATION, 1, 5)
	
};
