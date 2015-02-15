#include "classdefs.h"
#include "feature.h"
#include "map.h"
#include "tile.h"
#include "utils.h"
#include "world.h"

#include <math.h>

void world::pond(map * m, int x, int y, int size) {

    int c_x = x + size/2, c_y = y + size/2;
    for(int j = y; j < y + size && j < m->height; ++j) {
        for(int i = x; i < x + size && i < m->width; ++i) {
            if( sqrt((double)pow((double)(i - c_x),2.0) + pow((double)(j - c_y),2.0)) < size/2 ) {
                m->tiles[i][j].my_class = (entityclass *)tclass[TILE_WATER];
            }
        }
    }
}

void world::forest(map * m, int x, int y, int size) {

    int c_x = x + size/2, c_y = y + size/2;
    for(int j = y; j < y + size && j < m->height; ++j) {
        for(int i = x; i < x + size && i < m->width; ++i) {
            if( sqrt((double)pow((double)(i - c_x),2.0) + pow((double)(j - c_y),2.0)) < size/2 ) {
                tile * t = &(m->tiles[i][j]);
                if (t->my_actor == NULL && t->my_feature == NULL && t->has_flag(FLAG_TILE_CAN_WALK) && rand_int(4) == 0) {
                    t->my_feature = new feature(FEAT_TREE);
                }
            }
        }
    }
}

void world::hut(map *m, int x, int y, int size) {

}
