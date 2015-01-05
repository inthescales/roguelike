#include "featclass.h"

featclass::featclass(string n, symbol_code s, colorName c) {

    name = n;
    #if DISPLAY_TYPE == DISPLAY_CURSES || DISPLAY_TYPE == DISPLAY_WCURSES
    my_glyph = glyph(s, c);
    #endif
    mapentityclass::init();
    
    init();
}

void featclass::init() {
    mapentityclass::init();
}
