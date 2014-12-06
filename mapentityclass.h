#ifndef MAPENTITYCLASS_H
#define MAPENTITYCLASS_H

#include "entityclass.h"
#include "glyph.h"

using std::string;
using std::vector;

class mapentityclass : public entityclass {

    public:
    glyph my_glyph;
    
    // Functions ====
    mapentityclass();
    void init();
};
    

#endif
