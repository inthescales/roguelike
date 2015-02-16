#ifndef COLOR_H
#define COLOR_H

#include "enums.h"

class color {
    public:
    
    static short get_pair(short);
    static colorName get_grayed(short);
    static bool get_bold(short);
    static bool get_blink(short);
    static int get_attr(short);
};

#endif