#ifndef ENTITYCLASS_H
#define ENTITYCLASS_H

#include "effect.h"
#include "enums.h"
#include <string>
#include <vector>

#include "argmap.h"
#include "flagset.h"

using std::string;
using std::vector;

class effect;

class entityclass {

    public:
    int index;
    string name;
    std::map<trigger_t, vector<effect *> *> * trigger_effects;
    vector<timer_effect *> * timer_effects;
    statmap * stats;
    flagset * flags;
    
    // Functions =============
    entityclass();
    void init();
    int has_stat(stats_t);
    int get_stat(stats_t);
    bool has_flag(flags_t);
    
    vector<effect *> * get_effects(trigger_t);
    
    void add_stat(stats_t, int);
    void add_flag(flags_t);
    void add_trigger_effect(trigger_effect *);
};
    

#endif
