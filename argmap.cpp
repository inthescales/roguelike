#include "argmap.h"
#include "config.h"

argmap::argmap(){

}

argmap::argmap(argmap * copied) {

    the_map = copied->the_map;
}

// Functions for combining maps ================

// Add args from another map to this one.
// Original map is unchanged. Default to no replace;
void argmap::add_args(argmap * args, bool replace) {

	if (args == NULL) return;
	
	std::map<int, void*>::iterator it = the_map.begin();
	
	for (; it != the_map.end(); ++it) {
		
		if (args->has_value((args_t)it->first) && (!has_value((args_t)it->first) || replace)) {
			the_map[it->first] = it->second;
		}
	}
}

void argmap::add_args(argmap * args) {
	add_args(args, false);
}

// Functions for setting values ===========

bool argmap::add_int(args_t code, int int_in){
#ifdef size_64
  long temp = (long)int_in;
  the_map[code] = (void *)temp;
#else
  the_map[code] = (void *)int_in;
#endif
	return true;
}

bool argmap::add_entity(args_t code, entity * ent_in){
	the_map[code] = ent_in;
	return true;
}

bool argmap::add_actor(args_t code, actor * act_in){
	the_map[code] = act_in;
	return true;
}

bool argmap::add_object(args_t code, object * obj_in){
	the_map[code] = obj_in;
	return true;
}

bool argmap::add_feature(args_t code, feature * obj_in){
	the_map[code] = obj_in;
	return true;
}

bool argmap::add_tile(args_t code, tile * obj_in){
	the_map[code] = obj_in;
	return true;
}

bool argmap::add_condition(args_t code, condition * obj_in){
	the_map[code] = obj_in;
	return true;
}

// Function for checking whether value exists ======

bool argmap::has_value(args_t code){
	return the_map.count(code);
}

// Functions for getting values ==============

int argmap::get_int(args_t code){
#ifdef size_64
  long temp = (long)the_map[code];
  return (int)temp;
#else
	return (int)the_map[code];
#endif
}

entity * argmap::get_entity(args_t code) {
    if (has_value(code)) {
        return (entity *)the_map[code];
    }
    
    return NULL;
}

actor * argmap::get_actor(args_t code){
    if (has_value(code)) {
        return (actor *)the_map[code];
    }
    
    return NULL;
}

object * argmap::get_object (args_t code){
    if (has_value(code)) {
        return (object *)the_map[code];
    }
    
    return NULL;
}

feature * argmap::get_feature (args_t code){
    if (has_value(code)) {
        return (feature *)the_map[code];
    }
    
    return NULL;
}

tile * argmap::get_tile (args_t code){
    if (has_value(code)) {
        return (tile *)the_map[code];
    }
    
    return NULL;
}

condition * argmap::get_condition (args_t code){
    if (has_value(code)) {
        return (condition *)the_map[code];
    }
    
    return NULL;
}

// Stat functions =============================================
// These are identical to the above, duplicated for convenience

bool argmap::add_stat (stats_t code, int int_in){
#ifdef size_64
  long temp = (long)int_in;
  the_map[code] = (void *)temp;
#else
	the_map[code] = (void *)int_in;
#endif
	return true;
}

bool argmap::has_stat(stats_t code){
	return the_map.count(code);
}

int argmap::get_stat (stats_t code){
    if (has_stat(code)) {
        #ifdef size_64
            long temp = (long)the_map[code];
            return (int)temp;
        #else
            return (int)the_map[code];
        #endif
    }
    return 0;
}
