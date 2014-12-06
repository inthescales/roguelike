#ifndef ENTITYCLASS_H
#define ENTITYCLASS_H

#include "argmap.h"

#include "effect.h"
#include "enums.h"
#include <string>
#include <vector>

using std::string;
using std::vector;

class effect;
//class statmap; // Need argmap only temporarily for some reason I don't remember

class entityclass {

    public:
    int index;
    string name;
    std::map<trigger_t, vector<effect *> *> * trigger_effects;
    vector<timer_effect *> * timer_effects;
    statmap * stats;
    //flagmap * flags;
    
    // Functions =============
    entityclass();
    void init();
    int has_stat(stats_t);
    int get_stat(stats_t);
    
    vector<effect *> * get_effects(trigger_t);
    
    void add_stat(stats_t, int);
    void add_trigger_effect(trigger_effect *);
};
    

#endif
