#include "effect.h"
#include "objclass.h"

objclass::objclass(string tname, symbol_code tsym, colorName tcol, object_type ttyp, object_subtype tstyp,
				   material_t tmat, int tweigh, int tcost) {

	name = tname;
	assigned_name = "";
    #if DISPLAY_TYPE == DISPLAY_CURSES || DISPLAY_TYPE == DISPLAY_WCURSES
        my_glyph = glyph(tsym, tcol);
    #endif
	type = ttyp;
	subtype = tstyp;
	material = tmat;
	weight = tweigh;
	value = tcost;
	known = 0;
		
    mapentityclass::init();
}

void objclass::init() {
    mapentityclass::init();
}
