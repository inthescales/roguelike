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
    string name, assigned_name;
    vector<int> * actions;
    std::map<trigger_t, vector<effect *> *> * trigger_effects;
    vector<timer_effect *> * timer_effects;
    statmap * stats;
    flagset * flags;
    
    // Functions =============
    entityclass();
    void init();
    
    string get_name();
    
    int has_stat(stats_t);
    int get_stat(stats_t);
    int has_state(state_t);
    entityclass * get_state(state_t);
    bool has_flag(flags_t);
    
    vector<effect *> * get_effects(trigger_t);
    
    void add_stat(stats_t, int);
    void add_flag(flags_t);
    void add_state(state_t, entityclass *);
    void add_trigger_effect(trigger_effect *);
    void add_action(int);
};
    

#endif
