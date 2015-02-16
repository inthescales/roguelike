#include "actor.h"
#include "config.h"
#include "enums.h"
#include "map.h"
#include "tile.h"
#include "vision.h"

#include <cmath>

int multipliers[4][8] = {
    {1, 0, 0, -1, -1, 0, 0, 1},
    {0, 1, -1, 0, 0, -1, 1, 0},
    {0, 1, 1, 0, 0, -1, -1, 0},
    {1, 0, 0, 1, -1, 0, 0, -1}
};

bool vision::run_player_vision() {

    set<tile*> * tiles = tiles_in_view(act_player);
    
    last_seen_tiles = cur_seen_tiles;
    cur_seen_tiles = tiles;
        
    update_seen();
    
    return true;
}

set<tile*> * vision::tiles_in_view(actor * act) {

    set<tile*> * ret = new set<tile*>();
    
#if FOV_ALGORITHM == FOV_SHADOW
    ret = tiles_shadow(act);
#endif
    
    return ret;
}

set<tile*> * vision::tiles_shadow(actor * act) {

    map * m = act->current_map;
    int radius = act->get_stat(ASTAT_VISION_RANGE);
    set<tile*> * ret = new set<tile*>();
    
    if (radius >= 1) {
        ret->insert(&m->tiles[act->x][act->y]);
    }
    if (radius > 1) {
        
        for (uint i = 0; i < 8; i++) {
            set<tile*> * temp;
            temp = cast_light(m, act->x, act->y, radius, 1, 1.0, 0.0, multipliers[0][i],
                   multipliers[1][i], multipliers[2][i], multipliers[3][i]);
            ret->insert(temp->begin(), temp->end());
        }
    }
    
    return ret;
}


set<tile*> * vision::cast_light(map * m, uint x, uint y, uint radius, uint row,
        float start_slope, float end_slope, uint xx, uint xy, uint yx,
        uint yy) {
        
    set<tile*> * ret = new set<tile*>();
    if (start_slope < end_slope) {
        return ret;
    }
    float next_start_slope = start_slope;
    for (uint i = row; i <= radius; i++) {
        bool blocked = false;
        for (int dx = -i, dy = -i; dx <= 0; dx++) {
            float l_slope = (dx - 0.5) / (dy + 0.5);
            float r_slope = (dx + 0.5) / (dy - 0.5);
            if (start_slope < r_slope) {
                continue;
            } else if (end_slope > l_slope) {
                break;
            }

            int sax = dx * xx + dy * xy;
            int say = dx * yx + dy * yy;
            if ((sax < 0 && (uint)std::abs(sax) > x) ||
                    (say < 0 && (uint)std::abs(say) > y)) {
                continue;
            }
            uint ax = x + sax;
            uint ay = y + say;
            if (ax >= m->width || ay >= m->height) {
                continue;
            }

            uint radius2 = radius * radius;
            tile * cur_tile = &(m->tiles[ax][ay]); // Tile we care about at the moment
            
            if ((uint)(dx * dx + dy * dy) < radius2) {
                ret->insert(cur_tile);
            }

            if (blocked) {
                if (cur_tile->is_opaque()) {
                    next_start_slope = r_slope;
                    continue;
                } else {
                    blocked = false;
                    start_slope = next_start_slope;
                }
            } else if (cur_tile->is_opaque()) {
                blocked = true;
                next_start_slope = r_slope;
                cast_light(m, x, y, radius, i + 1, start_slope, l_slope, xx,
                        xy, yx, yy);
            }
        }
        if (blocked) {
            break;
        }
    }
    
    return ret;
}

// GENERAL FUNCTIONS ====================================

void vision::update_seen() {

    set<tile*>::iterator it;
    
    // Reveal new tiles
    if (cur_seen_tiles != NULL) {
        it = cur_seen_tiles->begin();
        for(; it != cur_seen_tiles->end(); ++it) {
            if (last_seen_tiles == NULL ||
                (last_seen_tiles != NULL && last_seen_tiles->count(*it) == 0)) {
                (*it)->known = (*it)->seen = true;
            }
        }
    }

    if (last_seen_tiles != NULL) {
        // Fog tiles no longer in view
        it = last_seen_tiles->begin();
        for(; it != last_seen_tiles->end(); ++it) {
            if (cur_seen_tiles != NULL && cur_seen_tiles->count(*it) == 0) {
                (*it)->seen = false;
            }
        }
    }
}
