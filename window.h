#ifndef WINDOW_H

#define WINDOW_H

#include "actor.h"
#include "map.h"

#include <string>
#include <vector>

using namespace std;

typedef vector<string> buffer;

class window{

	public:
	
	window(int, int, int, int);
	void clear();
	void display_map(map *);
	void display_status();
	void print(string, buffer *);
	void print(string);
	
	int x, y, width, height;

};

#endif
