#ifndef MAPENTITY_H
#define MAPENTITY_H

#include "entity.h"
#include "glyph.h"
#include "map.h"
#include <vector>

#include "argmap.h"
#include "flagset.h"

using std::vector;

class argmap;
class mapentityclass;
class timer;

class mapentity : public entity {

    public:
    map * current_map;
    int x, y;
    vector<condition *> * condition_list;

    // Functions ============
    mapentity();
    void init();
    void destroy();
    
    mapentityclass * get_class();
    string get_name_color();
    glyph get_glyph();
    symbol_code get_symbol();
	colorName get_color();
    
    int get_cond_stat(stats_t stat);
    int has_stat(stats_t);
    int get_stat(stats_t);
    bool has_cond_flag(flags_t stat);
    bool has_flag(flags_t);
    
    void resolve_trigger(trigger_t, argmap *);
    
    bool has_condition(int);
    condition * get_condition(int);
    bool add_condition(condition *);
    bool remove_condition(int);

};

#endif
