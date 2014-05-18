#include "actclass.h"

actclass::actclass(string n, unsigned char s, colorName c) {

	name = n;
	symbol = s;
	color = c;
	stats = new statmap();
}

void actclass::add_effect(trigger_effect n){

	effects.push_back(n);
}
