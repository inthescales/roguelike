#ifndef ENTITY_H
#define ENTITY_H

#include "entityclass.h"
#include "enums.h"
#include "timer.h"
#include <vector>

#include "argmap.h"
#include "flagset.h"

enum entity_type {
    ENT_TYPE_ACTOR = 0,
    ENT_TYPE_OBJECT,
    ENT_TYPE_FEATURE,
    ENT_TYPE_TILE,
    ENT_TYPE_CONDITION
};

using std::vector;

class argmap;
class map;

class entity {

    public:
    entityclass * my_class;
    vector<timer *> * timer_list;

    // Functions =======
    entity();
    void init();
    virtual void destroy() = 0;
    
    entityclass * get_class();
    string get_name();
    
    int has_stat(stats_t);
    int get_stat(stats_t);
    bool has_flag(flags_t);    
    
    void resolve_trigger(trigger_t, argmap *);
    
    void start_timers(map *);
    void destroy_timers(map *);
};

#endif
