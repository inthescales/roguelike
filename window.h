#ifndef WINDOW_H

#define WINDOW_H

#include "actor.h"
#include "map.h"

#include <string>

class window{

	public:
	
	window(int, int, int, int);
	void clear();
	void display_map(map *);
	void display_status();
	void print(std::string);
	//void print(std::string);
	
	int x, y, width, height;

};

#endif
