#ifndef PATHING_H
#define PATHING_H

#include "tile.h"

class pathing {

    public:
    static float distance_between(mapentity *, mapentity *);
    static vector<tile*> * line_between(mapentity *, mapentity *);
    static vector<tile*> * adjacent_to(tile *);
};

#endif