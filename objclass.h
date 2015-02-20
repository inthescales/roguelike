#ifndef OBJCLASS_H

#define OBJCLASS_H

#include "display.h"
#include "enums.h"
#include "glyph.h"
#include "object.h"
#include "mapentityclass.h"

#include <utility>
#include <string>
#include <vector>

using std::string;
using std::vector;

class argmap;
class trigger_effect;

typedef argmap statmap;

class objclass : public mapentityclass {

	public:
	
	object_type type;
	object_subtype subtype;
	material_t material;
	int weight;
	int value;
    bool known;
	
    // Functions ========
    
	objclass(string, symbol_code, colorName, object_type, object_subtype, material_t, int, int);	
    void init();
};

extern objclass * oclass[];

#endif
