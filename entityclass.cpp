#include "argmap.h"
#include "effect.h"
#include "entityclass.h"

// Setup and teardown ================================

entityclass::entityclass() {

    init();
}

void entityclass::init() {

	stats = new statmap();
    trigger_effects = new std::map<trigger_t, vector<effect *> *>();
    timer_effects = new vector<timer_effect *>();
}

// Stats management ==============================

int entityclass::has_stat(stats_t code) {

    return stats->has_stat(code);
}

int entityclass::get_stat(stats_t code) {

    return stats->get_stat(code);
}

// Effect and trigger management =========================

vector<effect *> * entityclass::get_effects(trigger_t trigger){

    return trigger_effects->at(trigger);
}

// Class construction ==============================

void entityclass::add_stat(stats_t code, int val) {

    stats->add_stat(code, val);
}

void entityclass::add_trigger_effect(trigger_effect * n){

    if (trigger_effects->count(n->trigger) == 0) {
        (*trigger_effects)[n->trigger] = new vector<effect *>();
    }
    
	trigger_effects->at(n->trigger)->push_back(n->eff);
}