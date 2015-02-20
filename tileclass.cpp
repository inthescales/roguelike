#include "classdefs.h"
#include "display.h"
#include "tileclass.h"

tileclass * tclass[TILE_MAX];

// Setup and teardown ============================

tileclass::tileclass(string nname, symbol_code s, colorName c) {
    name = nname;
    my_glyph = glyph(s, c);
}
