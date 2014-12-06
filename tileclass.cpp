#include "display.h"
#include "tileclass.h"

// Setup and teardown ============================

tileclass::tileclass(bool walk, bool swim, bool fly, bool opa, symbol_code s, colorName c) {
  can_walk = walk;
  can_swim = swim;
  can_fly = fly;
  opaque = opa;
  my_glyph = glyph(s, c);
}
