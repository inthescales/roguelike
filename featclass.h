#ifndef FEATCLASS_H

#define FEATCLASS_H

#include "config.h"
#include "enums.h"
#include "mapentityclass.h"

#include <vector>

class featclass : public mapentityclass {

	public:
	
    // Functions ===
	featclass(string, symbol_code, colorName);
    void init();	
};

#endif
