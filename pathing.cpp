#include "mapentity.h"
#include "pathing.h"


#include <math.h>

float pathing::distance_between(mapentity * orig, mapentity * dest) {

    float dist = floor(sqrt( pow( fabs(orig->x - dest->x), 2) + pow( fabs(orig->y - dest->y), 2) ));
    return dist;
}

vector<tile*> * pathing::line_between(mapentity * orig, mapentity * dest) {

    if(orig->current_map != dest->current_map) {
        // Something is horribly wrong
        return NULL;
    }
    
    map * m = orig->current_map;
    
    int x1 = orig->x, x2 = dest->x, y1 = orig->y, y2 = dest->y;
    vector<tile*> * ret = new vector<tile*>();
    
#if LINE_ALGORITHM == LINE_BRESENHAM
    // Bresenham's line algorithm
    
    const bool steep = (fabs(y2 - y1) > fabs(x2 - x1));
    float error;
    const float dx = fabs(x2 - x1);
    const float dy = fabs(y2 - y1);
    const int xstep = (x1 < x2) ? 1 : -1;
    const int ystep = (y1 < y2) ? 1 : -1;

    if (!steep) {
        error = dx / 2.0f;
        int y = (int)y1;
        const int xend = (x1 < x2) ? x2+1 : x2-1;
        for(int x=(int)x1; x != xend; x += xstep)
        {
        
            if(!(x == x1 && y == y1)) {
                ret->push_back(&m->tiles[x][y]);
            }

            error -= dy;
            if(error < 0)
            {
                y += ystep;
                error += dx;
            }

        }
    } else {
        error = dy / 2.0f;
        int x = (int)x1;
        const int yend = (y1 < y2) ? y2+1 : y2-1;
        for(int y=(int)y1; y != yend; y += ystep)
        {
        
            if(!(x == x1 && y == y1)) {
                ret->push_back(&m->tiles[x][y]);
            }

            error -= dx;
            if(error < 0)
            {
                x += xstep;
                error += dy;
            }

        }
    }
  
#endif

    return ret;

}

/*
 Get all tiles adjacent to the specified tile
*/
vector<tile*> * pathing::adjacent_to(tile * orig) {

    vector<tile*> * ret = new vector<tile*>();
    map * m = orig->current_map;
    
    for(int i = -1; i < 2; ++i) {
    
        for(int j = -1; j < 2; ++j) {

            int x = orig->x + i, y = orig->y + j;
            
            if(!(i == 0 && j == 0) &&
               x > 0 && y > 0 &&
               x < m->width && y < m->height) {
                ret->push_back(&m->tiles[x][y]);
            }
        }
    }
    
    return ret;
}