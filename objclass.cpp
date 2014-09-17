#include "effect.h"
#include "objclass.h"

objclass::objclass(string tname, symbol_code tsym, colorName tcol, object_type ttyp, object_subtype tstyp,
				   material_t tmat, int tweigh, int tcost) {

	name = tname;
	assigned_name = "";
	image = glyph(tsym, tcol);
	type = ttyp;
	subtype = tstyp;
	material = tmat;
	weight = tweigh;
	value = tcost;
	
	stats = new statmap();
	
	known = 0;
}

void objclass::add_effect(trigger_effect n){

	effects.push_back(n);
}

glyph objclass::get_glyph() {
  return image;
}

colorName objclass::get_color() {
  return image.get_color();
}
