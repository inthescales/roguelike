#ifndef VISION_H
#define VISION_H

#include <set>

using std::set;

typedef unsigned int uint;

class actor;

class vision {

public:
    static bool run_player_vision();
    static set<tile*> * tiles_in_view(actor *);
    static set<tile*> * tiles_shadow(actor *);
    static set<tile*> * cast_light(map *, uint, uint, uint, uint, float, float, uint, uint, uint, uint);
    
    static void update_seen();
};

#endif