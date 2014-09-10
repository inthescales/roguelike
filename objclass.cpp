#include "effect.h"
#include "objclass.h"

objclass::objclass(string tname, d_glyph tsym, colorName tcol, object_type ttyp, object_subtype tstyp,
				   material_t tmat, int tweigh, int tcost) {

	name = tname;
	assigned_name = "";
	symbol = tsym;
	color = tcol;
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
