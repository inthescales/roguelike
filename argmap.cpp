#include "argmap.h"

argmap::argmap(){
//	the_map = std::map<args_t, void *>();
}

bool argmap::has_value(args_t code){
	return the_map.count(code);
}

bool argmap::add_int(args_t code, int int_in){
	the_map[code] = (void *)int_in;
	return true;
}

int argmap::get_int(args_t code){
	return (int)the_map[code];
}

bool argmap::add_actor(args_t code, actor * act_in){
	the_map[code] = act_in;
	return true;
}

actor * argmap::get_actor(args_t code){
	return (actor *)the_map[code];
}

bool argmap::add_object(args_t code, object * obj_in){
	the_map[code] = obj_in;
	return true;
}

object * argmap::get_object (args_t code){
	return (object *)the_map[code];
}

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

// stat versions (same, just for convenience)

bool argmap::add_stat (stats_t code, int int_in){
	the_map[code] = (void *)int_in;
	return true;
}

int argmap::get_stat (stats_t code){
	return (int)the_map[code];
}

bool argmap::has_stat (stats_t code){
	return the_map.count(code);
}