#ifndef VISION_H
#define VISION_H

class actor;

typedef unsigned int uint;

class vision {

public:
    static bool run_player_vision();
    static vector<tile*> * tiles_in_view(actor *);
    static vector<tile*> * tiles_shadow(actor *);
    static vector<tile*> * cast_light(map *, uint, uint, uint, uint, float, float, uint, uint, uint, uint);
};

#endif