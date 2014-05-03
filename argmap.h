#ifndef ARGMAP_H

#define ARGMAP_H

#include "enums.h"
#include "globals.h"
#include <map>

enum args_t {
	ARG_AGENT = 0,
	ARG_OBJECT_USED
};

class argmap {

	private:
	std::map<int, void *> the_map;
	
	public:
	argmap();
	bool add_int(args_t, int);
	bool add_actor(args_t, actor *);
	//bool add_object(args_t, object *);
	//bool add_feature(args_t, feature *);
	//bool add_feature(args_t, tile *);
	
	bool has_value(args_t);
	int get_int(args_t);
	actor * get_actor(args_t);
	//object * get_object(args_t);
	//feature * get_feature(args_t);
	//tile * get_tile(args_t);
	
	// Stat functions (same thing really)
	bool add_stat(stats_t, int);
	int get_stat(stats_t);
	bool has_stat(stats_t);
};

typedef argmap statmap;

#endif

/*
	TODO (probably) - add bools, and have a flagmap typedef, for
	object and actor class flags.
*/