#include "glyph.h"

glyph::glyph() {

}

glyph::glyph(symbol_code ts, colorName cn) {
  symbol = ts;
  color = cn;
}

symbol_code glyph::get_symbol() {
  return symbol;
}

colorName glyph::get_color() {
  return color;
}
