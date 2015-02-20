#ifndef ACTCLASS_H

#define ACTCLASS_H

#include "argmap.h"
#include "display.h"
#include "effect.h"
#include "enums.h"
#include "glyph.h"
#include "mapentityclass.h"

#include <string>

using std::string;

class actclass : public mapentityclass {

	public:
	
	short ai;
	
    // Functions ===
	actclass(string, symbol_code, colorName);
    void init();
};

extern actclass * aclass[];

#endif
