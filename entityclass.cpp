#include "argmap.h"
#include "effect.h"
#include "entityclass.h"
#include "flagset.h"

// Setup and teardown ================================

entityclass::entityclass() {

    init();
}

void entityclass::init() {

    flags = new flagset();
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

// Flags management ==============================

bool entityclass::has_flag(flags_t code) {

    return flags->has_flag(code);
}

// Effect and trigger management =========================

vector<effect *> * entityclass::get_effects(trigger_t trigger){

    int size = trigger_effects->size();
    int test = trigger_effects->count(TRG_EAT);
    if (trigger_effects->count(trigger) > 0) {
        return trigger_effects->at(trigger);
    }
    
    return NULL;
}

// Class construction ==============================

void entityclass::add_stat(stats_t code, int val) {

    stats->add_stat(code, val);
}

void entityclass::add_flag(flags_t code) {

    flags->add_flag(code);
}

void entityclass::add_trigger_effect(trigger_effect * n){

    if (trigger_effects->count(n->trigger) == 0) {
        (*trigger_effects)[n->trigger] = new vector<effect *>();
    }
    
	trigger_effects->at(n->trigger)->push_back(n->eff);
}