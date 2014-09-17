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
  tclass[++c] = new tileclass(1, 0, 1, 0, symboldef[SYM_FLOOR], C_WHITE);

  //#GRASS
  tclass[++c] = new tileclass(1, 0, 1, 0, symboldef[SYM_GRASS], C_GREEN);

  //#WALL
  tclass[++c] = new tileclass(0, 0, 0, 1, symboldef[SYM_WALL], C_WHITE_INV);

  //#WATER
  tclass[++c] = new tileclass(0, 1, 1, 0, symboldef[SYM_WATER], C_BLUE);
}
