#include "argmap.h"

argmap::argmap(){
//	the_map = std::map<args_t, void *>();
}

bool argmap::add_actor (args_t code, actor * act_in){
	the_map[code] = act_in;
	return true;
}

actor * argmap::get_actor (args_t code){
	return (actor *)the_map[code];
}

bool argmap::add_int (args_t code, int int_in){
	the_map[code] = (void *)int_in;
	return true;
}

int argmap::get_int (args_t code){
	return (int)the_map[code];
}

bool argmap::add_stat (stats_t code, int int_in){
	the_map[code] = (void *)int_in;
	return true;
}

int argmap::get_stat (stats_t code){
	return (int)the_map[code];
}

bool argmap::has_value (args_t code){
	return the_map.count(code);
}

bool argmap::has_stat (stats_t code){
	return the_map.count(code);
}