#include "display.h"
#include "tileclass.h"

// Setup and teardown ============================

tileclass::tileclass(symbol_code s, colorName c) {
    my_glyph = glyph(s, c);
}
