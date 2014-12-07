#ifndef TILECLASS_H

#define TILECLASS_H

#include "config.h"
#include "display.h"
#include "enums.h"
#include "mapentityclass.h"

struct tileclass : public mapentityclass {

    tileclass(symbol_code, colorName);
};

#endif
