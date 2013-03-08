#include "graphics.h"
#include "tileclass.h"

struct tileclass tclass[] = {

	//#FLOOR
	TILE( 1, 0, 1 , 0, 250, C_WHITE),
	
	//#GRASS
	TILE( 1, 0, 1, 0, 250, C_GREEN),

	//#WALL
	TILE( 0, 0, 0, 1, 219, C_GRAY),
	
	//#WATER
	TILE( 0, 1, 1, 0, 247, C_BLUE)

};
