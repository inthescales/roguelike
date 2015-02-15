#include "display.h"
#include "globals.h"
//    z#include "effect.h"
#include "symboldefs.h"
#include "tileclass.h"
#include "tiledefs.h"

#include <utility>

void define_tiles() {

    int c = -1;

    //#FLOOR
    tclass[++c] = new tileclass("floor", symboldef[SYM_FLOOR], C_WHITE);
    tclass[c]->add_flag(FLAG_TILE_CAN_WALK);
    tclass[c]->add_flag(FLAG_TILE_CAN_FLY);

    //#GRASS
    tclass[++c] = new tileclass("grass", symboldef[SYM_GRASS], C_GREEN);
    tclass[c]->add_flag(FLAG_TILE_CAN_WALK);
    tclass[c]->add_flag(FLAG_TILE_CAN_FLY);
    
    //#WALL
    tclass[++c] = new tileclass("wall", symboldef[SYM_WALL], C_WHITE_INV);
    
    //#WATER
    tclass[++c] = new tileclass("water", symboldef[SYM_WATER], C_BLUE);
    tclass[c]->add_flag(FLAG_TILE_CAN_SWIM);
    tclass[c]->add_flag(FLAG_TILE_CAN_FLY);
}
