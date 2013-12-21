#include "actclass.h"

actclass::actclass(string n, unsigned char s, colorName c) {

	name = n;
	symbol = s;
	color = c;
	stats = new statmap();
}

void actclass::add_core_stats(int bdy, int mnd) {
	core_stat[CSTAT_BODY] = bdy;
	core_stat[CSTAT_MIND] = mnd;
}
