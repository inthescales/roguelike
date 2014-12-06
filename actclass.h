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
	
	std::string assigned_name;
	short ai;
	
    // Functions ===
	actclass(string, symbol_code, colorName);
    void init();
    
    glyph get_glyph();
    symbol_code get_symbol();
	colorName get_color();

};

#endif
