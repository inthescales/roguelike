#include "display.h"
#include "tileclass.h"

tileclass::tileclass(bool walk, bool swim, bool fly, bool opa, symbol_code s, colorName c) {
  can_walk = walk;
  can_swim = swim;
  can_fly = fly;
  opaque = opa;
  image = glyph(s, c);
}

glyph tileclass::get_glyph() {
  return image;
}

symbol_code tileclass::get_symbol() {
  return image.get_symbol();
}

colorName tileclass::get_color() {
  return image.get_color();
}
