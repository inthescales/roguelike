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
    
    return my_class->name;
}

// Trigger effect management ===================

void entity::resolve_trigger(trigger_t trigger, argmap * args)
{
    argmap * my_map = new argmap(args);
    my_map->add_entity(ARG_HOLDER_ENTITY, this);

    // Execute all these effects
    vector<effect*> * my_effects = get_class()->get_effects(trigger);
    for(int i = 0; i < my_effects->size(); ++i) {
        do_effect(my_map, my_effects->at(i));
    }
}

// Timer management =====================

void entity::start_timers(map * my_map) {
    
    // Create timers for timed effects in the condition's class
    std::vector<timer_effect *>::iterator it = my_class->timer_effects->begin();
    for (;it != get_class()->timer_effects->end(); ++it) {
        argmap * newmap = new argmap();
        newmap->add_entity(ARG_HOLDER_ENTITY, this);
        my_map->add_timer(new timer((*it)->eff, newmap, (*it)->time, (*it)->iterations, (*it)->delta));
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
