#include "entity.h"
#include "map.h"

// Setup and teardown ==================

entity::entity() {

    init();
}

void entity::init() {
    timer_list = new vector<timer *>();
}

// Basics ====================

entityclass * entity::get_class() {

    return my_class;
}

string entity::get_name() {
    if (individual_name.length() > 0) {
        return individual_name;
    } else {
        return my_class->get_name();
    }
}

// Trigger effect management ===================

void entity::resolve_trigger(trigger_t trigger, argmap * args)
{
    argmap * my_map = new argmap(args);
    my_map->add_entity(ARG_HOLDER_ENTITY, this);

    // Execute all these effects
    vector<effect*> * my_effects = get_class()->get_effects(trigger);
    if (my_effects != NULL) {
        for(int i = 0; i < my_effects->size(); ++i) {
            my_effects->at(i)->resolve(my_map);
        }
    }
}

// Timer management =====================

void entity::start_timers(map * my_map) {
    
    // Create timers for timed effects in the condition's class
    std::vector<timer_effect *>::iterator it = my_class->timer_effects->begin();
    for (;it != get_class()->timer_effects->end(); ++it) {
        argmap * newmap = new argmap();
        newmap->add_entity(ARG_HOLDER_ENTITY, this);
        my_map->add_timer(new timer(*it, newmap));
    }
}

void entity::destroy_timers(map * my_map) {
    
    vector<timer *>::iterator it = timer_list->begin();
    
    for(; it != timer_list->end(); ++it) {
    
        my_map->remove_timer(*it);
    }
}

// Stat management ==================

int entity::has_stat(stats_t code) {

    return get_class()->has_stat(code);
}

int entity::get_stat(stats_t code) {

    return get_class()->get_stat(code);
    
    return 0;
}

// Flag management ================

bool entity::has_flag(flags_t code) {

    return get_class()->has_flag(code);
}

// State management ===============

int entity::has_state(state_t code) {

    return get_class()->has_state(code);
}

entityclass * entity::get_state(state_t code) {

    return get_class()->get_state(code);
    
    return NULL;
}

bool entity::change_state(state_t code) {

    entityclass * new_state = get_state(code);
    if (new_state != NULL) {
        my_class = new_state;
        return true;
    }

    return false;
}

// Action management =======================

vector<int> * entity::get_actions() {

    vector<int> * r = new vector<int>(*(get_class()->actions));
    return r;
}