#include "argmap.h"

argmap::argmap(){
//	the_map = std::map<args_t, void *>();
}

bool argmap::add_actor (args_t code, actor * act_in){
	the_map[code] = act_in;
}

actor * argmap::get_actor (args_t code){
	return (actor *)the_map[code];
}