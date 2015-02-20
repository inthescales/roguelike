#include "condclass.h"
#include "conddefs.h"
#include "display.h"
#include "effect.h"

#include <utility>

using std::pair;
using std::vector;

// Reminder -
// condclass(string nname, colorName ncolor, int nstrength, int nduration, int ninterval, int nvalue,
// cond_stack_policy_t nstackpol, cond_timer_policy_t ntimerpol)

void define_conditions(){

	int c = -1;

	//#CURSED
	cclass[++c] = new condclass("cursed", C_RED, 1, 30, -1, 0, 0, -1, 1, false, true, CSPOL_ADD, CTPOL_ADD);
	cclass[c]->add_stat(ASTAT_BODY, -1);
    cclass[c]->add_stat(ASTAT_MIND, -1);
	
}
