#include "objclass.h"
#include "effect.h"

objclass::objclass(string tname, unsigned char tsym, colorName tcol, object_type ttyp, object_subtype tstyp,
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
	
	known = 0;
}

void objclass::add_effect(trigger_effect n){

	effects.push_back(n);
}