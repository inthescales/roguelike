#include "argmap.h"
#include "config.h"

argmap::argmap(){
//	the_map = std::map<args_t, void *>();
}

bool argmap::has_value(args_t code){
	return the_map.count(code);
}

bool argmap::add_int(args_t code, int int_in){
#ifdef size_64
  long temp = (long)int_in;
  the_map[code] = (void *)temp;
#else
  the_map[code] = (void *)int_in;
#endif
	return true;
}

int argmap::get_int(args_t code){
#ifdef size_64
  long temp = (long)the_map[code];
  return (int)temp;
#else
	return (int)the_map[code];
#endif
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

actor * argmap::get_actor(args_t code){
	return (actor *)the_map[code];
}

object * argmap::get_object (args_t code){
	return (object *)the_map[code];
}

feature * argmap::get_feature (args_t code){
	return (feature *)the_map[code];
}

tile * argmap::get_tile (args_t code){
	return (tile *)the_map[code];
}

condition * argmap::get_condition (args_t code){
	return (condition *)the_map[code];
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
#ifdef size_64
  long temp = (long)int_in;
  the_map[code] = (void *)temp;
#else
	the_map[code] = (void *)int_in;
#endif
	return true;
}

int argmap::get_stat (stats_t code){
#ifdef size_64
  long temp = (long)the_map[code];
  return (int)temp;
#else
	return (int)the_map[code];
#endif
}

bool argmap::has_stat (stats_t code){
	return the_map.count(code);
}
