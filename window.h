#ifndef WINDOW_H

#define WINDOW_H

#include <string>
#include <vector>

using std::string;
using std::vector;
typedef vector<string> buffer;

class actor;
class object;
class map;

class window{

	public:
	
	window(int, int, int, int);
	void clear();
	void display_map(map *);
	void display_status();
	void display_inventory(actor &);
	void display_equipment(actor &);
	void display_conditions(actor *);
	void print(string);
	void print_twin(string, string);
	void print(string, buffer &);
	void print_buf(buffer &);
	void print_line(string, int);
	vector<object*> menu_select_objects(vector<object*> &, bool, bool);
	
	int x, y, width, height;

};

#endif
