#ifndef WINDOW_H

#define WINDOW_H

#include "actor.h"
#include "map.h"

#include <string>
#include <vector>

using namespace std;

typedef vector<string> buffer;

class object;

class window{

	public:
	
	window(int, int, int, int);
	void clear();
	void display_map(map *);
	void display_status();
	void display_inventory(actor &);
	void print(string);
	void print(string, buffer &);
	void print_buf(buffer &);
	void print_line(string, int);
	vector<object*> menu_select_objects(vector<object*> &);
	
	int x, y, width, height;

};

#endif
