#include "display.h"
#include "tileclass.h"

// Setup and teardown ============================

tileclass::tileclass(string nname, symbol_code s, colorName c) {
    name = nname;
    my_glyph = glyph(s, c);
}
